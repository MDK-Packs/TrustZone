/*
 * FreeRTOS STM32 Reference Integration
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

/**
 * @file mbedtls_transport.h
 * @brief TLS transport interface header.
 */

#ifndef _MBEDTLS_TRANSPORT_H
#define _MBEDTLS_TRANSPORT_H

#include "mbedtls_error_utils.h"

/* mbed TLS includes. */
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ssl.h"
#include "mbedtls/threading.h"
#include "mbedtls/x509.h"
#include "mbedtls/pk_internal.h"

#include "tls_transport_config.h"

#include "PkiObject.h"

#ifdef MBEDTLS_TRANSPORT_PKCS11
#include "core_pkcs11_config.h"
#include "core_pkcs11.h"
#endif /* MBEDTLS_TRANSPORT_PKCS11 */

#ifdef MBEDTLS_TRANSPORT_PSA
#include "psa/crypto.h"
#include "psa/internal_trusted_storage.h"
#include "psa/protected_storage.h"
#endif /* MBEDTLS_TRANSPORT_PSA */


/*-----------------------------------------------------------*/

#ifdef MBEDTLS_TRANSPORT_PKCS11
extern mbedtls_pk_info_t mbedtls_pkcs11_pk_ecdsa;
extern mbedtls_pk_info_t mbedtls_pkcs11_pk_rsa;

int32_t lReadCertificateFromPKCS11( mbedtls_x509_crt * pxCertificateContext,
                                    CK_SESSION_HANDLE xP11SessionHandle,
                                    const char * pcCertificateLabel,
                                    size_t xLabelLen );

int32_t lWriteCertificateToPKCS11( const mbedtls_x509_crt * pxCertificateContext,
                                   CK_SESSION_HANDLE xP11SessionHandle,
                                   char * pcCertificateLabel,
                                   size_t uxCertificateLabelLen );

int32_t lWriteEcPublicKeyToPKCS11( const mbedtls_pk_context * pxPubKeyContext,
                                   CK_SESSION_HANDLE xP11SessionHandle,
                                   char * pcPubKeyLabel,
                                   size_t uxPubKeyLabelLen );

CK_RV xPKCS11_initMbedtlsPkContext( mbedtls_pk_context * pxMbedtlsPkCtx,
                                    CK_SESSION_HANDLE xSessionHandle,
                                    CK_OBJECT_HANDLE xPkHandle );

const char * pcPKCS11StrError( CK_RV xError );

int lPKCS11RandomCallback( void * pvCtx,
                           unsigned char * pucOutput,
                           size_t uxLen );

int lPKCS11PkMbedtlsCloseSessionAndFree( mbedtls_pk_context * pxMbedtlsPkCtx );

#endif /* MBEDTLS_TRANSPORT_PKCS11 */

#ifdef MBEDTLS_TRANSPORT_PSA

int32_t lWriteCertificateToPSACrypto( psa_key_id_t xCertId,
                                      const mbedtls_x509_crt * pxCertificateContext );

psa_status_t xReadObjectFromPSACrypto( unsigned char ** ppucObject,
                                       size_t * puxObjectLen,
                                       psa_key_id_t xCertId );

psa_status_t xReadPublicKeyFromPSACrypto( unsigned char ** ppucPubKeyDer,
                                          size_t * puxPubDerKeyLen,
                                          psa_key_id_t xKeyId );

int32_t lWritePublicKeyToPSACrypto( psa_key_id_t xPubKeyId,
                                    const mbedtls_pk_context * pxPublicKeyContext );

int32_t lReadCertificateFromPSACrypto( mbedtls_x509_crt * pxCertificateContext,
                                       psa_key_id_t xCertId );

int32_t lWriteObjectToPsaIts( psa_storage_uid_t xObjectUid,
                              const uint8_t * pucData,
                              size_t uxDataLen );

int32_t lReadObjectFromPsaIts( uint8_t ** ppucData,
                               size_t * puxDataLen,
                               psa_storage_uid_t xObjectUid );

int32_t lWriteObjectToPsaPs( psa_storage_uid_t xObjectUid,
                             const uint8_t * pucData,
                             size_t uxDataLen );

int32_t lReadObjectFromPsaPs( uint8_t ** ppucData,
                              size_t * puxDataLen,
                              psa_storage_uid_t xObjectUid );

int32_t lWriteCertificateToPsaIts( psa_storage_uid_t xCertUid,
                                   const mbedtls_x509_crt * pxCertificateContext );

int32_t lReadCertificateFromPsaIts( mbedtls_x509_crt * pxCertificateContext,
                                    psa_storage_uid_t xCertUid );

int32_t lWriteCertificateToPsaPS( psa_storage_uid_t xCertUid,
                                  const mbedtls_x509_crt * pxCertificateContext );

int32_t lReadCertificateFromPsaPS( mbedtls_x509_crt * pxCertificateContext,
                                   psa_storage_uid_t xCertUid );

int lPSARandomCallback( void * pvCtx,
                        unsigned char * pucOutput,
                        size_t uxLen );

int32_t lPsa_initMbedtlsPkContext( mbedtls_pk_context * pxMbedtlsPkCtx,
                                   psa_key_id_t xKeyId );

#endif /* MBEDTLS_TRANSPORT_PSA */

#endif /* _MBEDTLS_TRANSPORT_H */
