/* -----------------------------------------------------------------------------
 * Copyright (c) 2021 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * -------------------------------------------------------------------------- */

#include <stdio.h>
#include "main.h"
#include "cmsis_os2.h"
#include "cmsis_vio.h"
#include "FreeRTOS.h"
#include "iot_crypto.h"
#include "iot_logging_task.h"
#include "aws_demo.h"
#include "types/iot_network_types.h"
#include "aws_iot_network_config.h"
#include "aws_clientcredential.h"
#include "core_pkcs11.h"
#include "core_pkcs11_config.h"
#include "pkcs11.h"
#include "kvstore.h"
#include "cli.h"

/* Set logging task as high priority task */
#define LOGGING_TASK_PRIORITY                         (tskIDLE_PRIORITY)
#define LOGGING_TASK_STACK_SIZE                       (1440)
#define LOGGING_MESSAGE_QUEUE_LENGTH                  (32)

extern int32_t ns_interface_lock_init(void);
extern int32_t socket_startup (void);

static const osThreadAttr_t app_main_attr = {
  .stack_size = 4096U
};

#if (configAPPLICATION_ALLOCATED_HEAP == 1U)
#if !(defined(configHEAP_REGION0_ADDR) && (configHEAP_REGION0_ADDR != 0U))
static uint8_t heap_region0[configHEAP_REGION0_SIZE] __ALIGNED(8);
#endif

const HeapRegion_t xHeapRegions[] = {
#if defined(configHEAP_REGION0_ADDR) && (configHEAP_REGION0_ADDR != 0U)
 { (uint8_t *)configHEAP_REGION0_ADDR, configHEAP_REGION0_SIZE },
#else
 { (uint8_t *)heap_region0, configHEAP_REGION0_SIZE },
#endif
 { NULL, 0 }
};
#endif

int32_t FindPKCS11Keys (void) {
  int32_t dev_key_priv = 0;
  int32_t dev_key_pub = 0;
  int32_t dev_cert = 0;
#ifdef CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED
  int32_t ota_key_pub = 0;
#endif
  CK_RV xResult;
  CK_SESSION_HANDLE xSession = 0;
  CK_FUNCTION_LIST_PTR pxFunctionList;
  CK_OBJECT_HANDLE xHandle;

  xResult = C_GetFunctionList(&pxFunctionList);

  if (xResult == CKR_OK) {
    xResult = xInitializePkcs11Session(&xSession);
  }

  if (xResult == CKR_OK) {
    xResult = xFindObjectWithLabelAndClass(xSession,
                                           pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                           strlen(pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS),
                                           CKO_PRIVATE_KEY,
                                           &xHandle);
    if ((xResult == CKR_OK) && (xHandle != 0)) {
      dev_key_priv = 1;
    }
  }

  if (xResult == CKR_OK) {
    xResult = xFindObjectWithLabelAndClass(xSession,
                                           pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                           strlen(pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS),
                                           CKO_PUBLIC_KEY,
                                           &xHandle);
    if ((xResult == CKR_OK) && (xHandle != 0)) {
      dev_key_pub = 1;
    }
  }

  if (xResult == CKR_OK) {
    xResult = xFindObjectWithLabelAndClass(xSession,
                                           pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                           strlen(pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS),
                                           CKO_CERTIFICATE,
                                           &xHandle);
    if ((xResult == CKR_OK) && (xHandle != 0)) {
      dev_cert = 1;
    }
  }

#ifdef CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED
  if (xResult == CKR_OK) {
    xResult = xFindObjectWithLabelAndClass(xSession,
                                           pkcs11configLABEL_CODE_VERIFICATION_KEY,
                                           strlen(pkcs11configLABEL_CODE_VERIFICATION_KEY),
                                           CKO_PUBLIC_KEY,
                                           &xHandle);
    if ((xResult == CKR_OK) && (xHandle != 0)) {
      ota_key_pub = 1;
    }
  }
#endif

  if (xSession) {
    pxFunctionList->C_CloseSession(xSession);
  }

  if ((dev_key_priv == 0) || (dev_key_pub == 0) || (dev_cert == 0)) {
    return 0;
  }
#ifdef CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED
  if (ota_key_pub == 0) {
    return 0;
  }
#endif

  return 1;
}

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
static void app_main (void *argument) {
  int32_t provision;
  uint32_t value;
  int32_t status;

  (void)argument;

  KVStore_init();

  KVStore_getString(CS_CORE_THING_NAME, pcIOT_THING_NAME, sizeof(pcIOT_THING_NAME));
  KVStore_getString(CS_CORE_MQTT_ENDPOINT, pcMQTT_BROKER_ENDPOINT, sizeof(pcMQTT_BROKER_ENDPOINT));
  KVStore_getString(CS_WIFI_SSID, pcWIFI_SSID, sizeof(pcWIFI_SSID));
  KVStore_getString(CS_WIFI_CREDENTIAL, pcWIFI_PASSWORD, sizeof(pcWIFI_PASSWORD));
  value = KVStore_getUInt32(CS_CORE_MQTT_PORT, NULL);
  usMQTT_BROKER_PORT = (uint16_t)value;

  provision = 0;
  if (FindPKCS11Keys() == 0) {
    provision = 1;
  }
  if ((pcIOT_THING_NAME[0] == 0) || (pcMQTT_BROKER_ENDPOINT[0] == 0)) {
    provision = 1;
  }
#if ( ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
  if ((pcWIFI_SSID[0] == 0) || (pcWIFI_PASSWORD[0] == 0)) {
    provision = 1;
  }
#endif

  if (provision || vioGetSignal(vioBUTTON0)) {
    printf( "Command Line Interface (CLI) to provision the device.\r\n" );
    CRYPTO_Init();
    xTaskCreate( Task_CLI, "cli", 2048, NULL, 1, NULL );
    osDelay(osWaitForever);
    for (;;) {}
  }

  status = socket_startup();

  if (status == 0) {
    /* Start demos. */
    DEMO_RUNNER_RunDemos();
  }

  osDelay(osWaitForever);
  for (;;) {}
}

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
void app_initialize (void) {

#if (configAPPLICATION_ALLOCATED_HEAP == 1U)
  vPortDefineHeapRegions (xHeapRegions);
#endif

  /* Create logging task */
  xLoggingTaskInitialize (LOGGING_TASK_STACK_SIZE,
                          LOGGING_TASK_PRIORITY,
                          LOGGING_MESSAGE_QUEUE_LENGTH);

  ns_interface_lock_init();

  osThreadNew(app_main, NULL, &app_main_attr);
}
