/*
 * Copyright (c) 2020-2021 Arm Limited. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* Key provisioning include. */
#include "ota_provision.h"

#include "ota_demo_config.h"
#include "tls_transport_config.h"

/* Mbedtls */
#include "mbedtls/pem.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509.h"
#include "mbedtls/pk.h"
#include "mbedtls/platform.h"
#include "mbedtls/ecp.h"

static const unsigned char cOTA_SignCert[] = otapalconfigCODE_SIGNING_CERTIFICATE;

int ota_provision (void)
{
  unsigned char * pucPubKeyBuffer = NULL;
  size_t uxPubKeyBufferLen = 0;
  mbedtls_x509_crt xCrt;
  mbedtls_ecdsa_context * pxEcpKey;
  mbedtls_pk_type_t xKeyType;
  psa_ecc_family_t xKeyFamily;
  psa_status_t xStatus = PSA_SUCCESS;
  psa_key_attributes_t xKeyAttributes = PSA_KEY_ATTRIBUTES_INIT;
  int lResult;

  mbedtls_x509_crt_init(&xCrt);

  lResult = mbedtls_x509_crt_parse(&xCrt, &cOTA_SignCert[0], sizeof(cOTA_SignCert));
  if (lResult != 0)
  {
    return lResult;
  }

  xKeyType = mbedtls_pk_get_type(&xCrt.pk);

  if (xKeyType == MBEDTLS_PK_ECKEY)
  {
    pxEcpKey = (mbedtls_ecdsa_context *)xCrt.pk.pk_ctx;
    switch(pxEcpKey->grp.id)
    {
      case MBEDTLS_ECP_DP_SECP192R1:
      case MBEDTLS_ECP_DP_SECP224R1:
      case MBEDTLS_ECP_DP_SECP256R1:
      case MBEDTLS_ECP_DP_SECP384R1:
      case MBEDTLS_ECP_DP_SECP521R1:
        xKeyFamily = PSA_ECC_FAMILY_SECP_R1;
        break;
      case MBEDTLS_ECP_DP_BP256R1:
      case MBEDTLS_ECP_DP_BP384R1:
      case MBEDTLS_ECP_DP_BP512R1:
        xKeyFamily = PSA_ECC_FAMILY_BRAINPOOL_P_R1;
        break;
      case MBEDTLS_ECP_DP_CURVE25519:
      case MBEDTLS_ECP_DP_CURVE448:
        xKeyFamily = PSA_ECC_FAMILY_MONTGOMERY;
        break;
      case MBEDTLS_ECP_DP_SECP192K1:
      case MBEDTLS_ECP_DP_SECP224K1:
      case MBEDTLS_ECP_DP_SECP256K1:
        xKeyFamily = PSA_ECC_FAMILY_SECP_K1;
        break;
      default:
        xKeyFamily = 0;
        break;
    }

    psa_set_key_id(&xKeyAttributes, OTA_SIGNING_KEY_ID);
    psa_set_key_type(&xKeyAttributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(xKeyFamily));
    psa_set_key_algorithm(&xKeyAttributes, PSA_ALG_ECDSA(PSA_ALG_ANY_HASH));
    psa_set_key_lifetime(&xKeyAttributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                          (
                                            PSA_KEY_LIFETIME_PERSISTENT,
                                            PSA_KEY_LOCATION_LOCAL_STORAGE)
                                          );
    psa_set_key_bits(&xKeyAttributes, mbedtls_pk_get_bitlen(&xCrt.pk));
    psa_set_key_usage_flags(&xKeyAttributes, PSA_KEY_USAGE_VERIFY_MESSAGE |
                                             PSA_KEY_USAGE_VERIFY_HASH    |
                                             PSA_KEY_USAGE_EXPORT );

    uxPubKeyBufferLen = PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(xKeyAttributes.bits);

    pucPubKeyBuffer = mbedtls_calloc(1, uxPubKeyBufferLen);

    if(pucPubKeyBuffer == NULL)
    {
      xStatus = PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    else
    {
      /* Write EC point to buffer */
      lResult = mbedtls_ecp_point_write_binary(&(pxEcpKey->grp),
                                               &(pxEcpKey->Q),
                                               MBEDTLS_ECP_PF_UNCOMPRESSED,
                                               &uxPubKeyBufferLen,
                                               pucPubKeyBuffer,
                                               uxPubKeyBufferLen );
      if(lResult != 0)
      {
        xStatus = PSA_ERROR_GENERIC_ERROR;
      }
    }
  }
  else
  {
    xStatus = PSA_ERROR_NOT_SUPPORTED;
  }

  /* Export key to PSA */
  if(xStatus == PSA_SUCCESS)
  {
    xStatus = psa_import_key(&xKeyAttributes,
                             pucPubKeyBuffer,
                             uxPubKeyBufferLen,
                             &(xKeyAttributes.id));
  }
  mbedtls_free(pucPubKeyBuffer);

  switch(xStatus)
  {
    case PSA_SUCCESS:
      break;
    case PSA_ERROR_NOT_SUPPORTED:
      return(MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE);
    case PSA_ERROR_INSUFFICIENT_MEMORY:
      return(MBEDTLS_ERR_PK_ALLOC_FAILED);
    case PSA_ERROR_INSUFFICIENT_ENTROPY:
      return(MBEDTLS_ERR_ECP_RANDOM_FAILED);
    case PSA_ERROR_BAD_STATE:
      return(MBEDTLS_ERR_PK_BAD_INPUT_DATA);
    /* All other failures */
    case PSA_ERROR_COMMUNICATION_FAILURE:
    case PSA_ERROR_HARDWARE_FAILURE:
    case PSA_ERROR_CORRUPTION_DETECTED:
      return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    default: /* We return the same as for the 'other failures',
              * but list them separately nonetheless to indicate
              * which failure conditions we have considered. */
      return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
  }
  return 0;
}
