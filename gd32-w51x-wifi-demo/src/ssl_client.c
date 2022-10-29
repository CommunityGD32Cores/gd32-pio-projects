/*!
    \file    ssl_client.c
    \brief   SSL client demonstration program for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "ssl_tz.h"  /* must be included before mbedtls header file */
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "wrapper_os.h"
#include "init_rom.h"
#include <string.h>

#if defined(CONFIG_TZ_ENABLED)
#include "mbedtls_nsc.h"
#endif

#ifdef CONFIG_SSL_TEST
/*-----------------Options -----------------------*/
#define TLS_VERIFY_SRV_CERT        1
#define TLS_VERIFY_CLI_CERT        0

#define SERVER_PORT "443"
#define SERVER_NAME "www.baidu.com"
#define HTTP_PROTOCOL "HTTP/1.0\r\n"

/* TLS_CRT_[Chain Depth]_[Sign Algorithm]_[Key Len or Curve Name]_[Cert Digest Algorithm] */
#define TLS_CRT_NONE                            0   //no cert
#define TLS_CRT_1_RSA_1024_SHA256               1    //rsa1
#define TLS_CRT_1_RSA_2048_SHA1                 2    //rsa2
#define TLS_CRT_1_RSA_3072_SHA256               3    //rsa3
#define TLS_CRT_1_ECDSA_PRIME256V1_SHA256       4    //ecp1
#define TLS_CRT_1_ECDSA_SECP384R1_SHA384        5    //ecp2
#define TLS_CRT_1_ECDSA_BRAINP512R1_SHA512      6    //ecp3
#define TLS_CRT_1_ECDSA_SECP521R1_SHA512        7    //ecp4
#define TLS_CRT_3_RSA_2048_SHA512               8    //rsa_chain
#define TLS_CRT_3_ECDSA_SECP521R1_SHA512        9    //ecp_chain
#define BAIDU_CERT                              10
#define TLS_CRT_MAX_INDEX                       10

#define HTTPS_GET         1
#define HTTPS_HEAD        2
#define HTTPS_OPTIONS     3
#define HTTPS_TRACE       4
#define HTTPS_POST        5

#if (TLS_VERIFY_SRV_CERT || TLS_VERIFY_CLI_CERT)
#define TLS_CRT_USED
#include "ssl_certs.c.in"
#endif

static char server_host[128];
static char server_path[128];
static char server_port[8];
static char https_request_body[128];
#ifdef TLS_CRT_USED
static unsigned int cert_type;
#endif
unsigned int debug_level = 3;
unsigned int wait_srv_rsp = 1;
uint8_t https_method;

/*!
    \brief      owner debug function
    \param[in]  file: pointer to the file
    \param[in]  line: line value
    \param[in]  str: pointer to the string
    \param[out] none
    \retval     none
*/
static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);
    printf("%s:%04d: %s", file, line, str );
}
/*!
    \brief      owner get random function
    \param[in]  p_rng:
    \param[in]  output_len: get random calue length
    \param[out] output: pointer to the output value
    \retval     function run state(0: no error, other: have error)
*/
static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
    sys_random_bytes_get(output, output_len);
    return 0;
}
#if TLS_VERIFY_SRV_CERT
/*!
    \brief      owner verify function
    \param[in]  data: pointer to the input value
    \param[in]  crt: pointer to the X.509 certificates structures
    \param[in]  depth: depth vlaue
    \param[in]  flags: pointer to the flag value
    \param[out] output: pointer to the output value
    \retval     function run state(0: no error, other: have error)
*/
static int my_verify(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    char buf[1024];
    ((void) data);

    printf("Verify requested for (Depth %d):\r\n", depth);
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    printf("%s", buf);

    if(((*flags) & MBEDTLS_X509_BADCERT_EXPIRED) != 0)
        printf("server certificate has expired\r\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_REVOKED) != 0)
        printf("  ! server certificate has been revoked\r\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0)
        printf("  ! CN mismatch\r\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0)
        printf("  ! self-signed or not signed by a trusted CA\r\n");

    if(((*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED) != 0)
        printf("  ! CRL not trusted\r\n");

    if(((*flags) & MBEDTLS_X509_BADCRL_EXPIRED) != 0)
        printf("  ! CRL expired\r\n");

    if(((*flags) & MBEDTLS_X509_BADCERT_OTHER) != 0)
        printf("  ! other (unknown) flag\r\n");

    if((*flags) == 0)
        printf("  Certificate verified without error flags\r\n");

    return(0);
}
#endif
/* sizeof(mbedtls_net_context) = 4
    sizeof(mbedtls_ssl_context) = 312
    sizeof(mbedtls_ssl_config) = 208
    sizeof(mbedtls_x509_crt) = 308
    sizeof(mbedtls_pk_context) = 8
    SSL client STACK = 2048 dword, high water mark = 1822 dword
*/
/*!
    \brief      ssl client
    \param[in]  arg: pointer to the input value
    \param[out] none
    \retval     none
*/
void ssl_client(void *arg)
{
    int ret, len;
    mbedtls_net_context server_fd;
    unsigned int flags;
    unsigned char buf[1025];

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

#ifdef TLS_CRT_USED
    int need_cert = FALSE;
#endif
#if TLS_VERIFY_SRV_CERT
    mbedtls_x509_crt ca_cert;
#endif
#if TLS_VERIFY_CLI_CERT
    mbedtls_x509_crt cli_cert;
    mbedtls_pk_context cli_key;
#endif

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( debug_level );
#endif

    /*
    * 0. Initialize the RNG and the session data
    */
#ifndef CONFIG_TZ_ENABLED  //  MBL has called it yet
    mbedtls_ecp_curve_val_init();
#endif
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
#if TLS_VERIFY_SRV_CERT
    mbedtls_x509_crt_init( &ca_cert );
#endif
#if TLS_VERIFY_CLI_CERT
    mbedtls_x509_crt_init( &cli_cert );
    mbedtls_pk_init( &cli_key );
#endif

    printf( "  . Seeding the random number generator...\r\n" );

    /*
    * 1. Initialize certificates
    */
#ifdef TLS_CRT_USED
    need_cert = ssl_client_crt_init(cert_type);
#endif
#if TLS_VERIFY_SRV_CERT
    if(need_cert){
        printf( "  . Loading the CA root certificate ..." );

        ret = mbedtls_x509_crt_parse(&ca_cert, (const unsigned char *)ca_cert_str, strlen(ca_cert_str) + 1);
        if( ret < 0 ){
            printf( " failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x\r\n", -ret );
            goto exit;
        }

        printf( " ok (%d skipped)\r\n", ret );
    }
#endif

#if TLS_VERIFY_CLI_CERT
    if(need_cert){
        printf( "  . Loading the Client certificate ..." );

        ret = mbedtls_x509_crt_parse(&cli_cert, (const unsigned char *)cli_cert_str, strlen(cli_cert_str) + 1);
        if( ret < 0 ){
            printf( " failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x\r\n", -ret );
            goto exit;
        }

        printf( " ok (%d skipped)\r\n", ret );

        printf( "  . Loading the Client key ..." );

        ret = mbedtls_pk_parse_key(&cli_key, (const unsigned char *)cli_key_str, strlen(cli_key_str) + 1, NULL, 0);
        if( ret < 0 ){
            printf( " failed\r\n  !  mbedtls_pk_parse_key returned -0x%x\r\n", -ret );
            goto exit;
        }

        printf( " ok (%d skipped)\r\n", ret );
    }
#endif

    /*
    * 2. Start the connection
    */
    printf( "  . Connecting to tcp/%s/%s...", server_host, server_port );

    if( ( ret = mbedtls_net_connect( &server_fd, server_host,
        server_port, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        printf( " failed\r\n  ! mbedtls_net_connect returned %d\r\n", ret );
        goto exit;
    }

    printf( " ok\r\n" );

    /*
    * 3. Setup stuff
    */
    printf( "  . Setting up the SSL/TLS structure..." );

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        printf( " failed\r\n  ! mbedtls_ssl_config_defaults returned %d\r\n", ret );
        goto exit;
    }

    printf( " ok\r\n" );

    /* OPTIONAL is not optimal for security,
    * but makes interop easier in this simplified example */
#if defined(CONFIG_TZ_ENABLED)
    mbedtls_ssl_conf_rng( &conf, mbedtls_random_nsc, NULL);
#else
    mbedtls_ssl_conf_rng( &conf, my_random, NULL);
#endif
    mbedtls_ssl_conf_dbg( &conf, my_debug, NULL );

#if TLS_VERIFY_SRV_CERT
    if(need_cert)
    {
        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, NULL);
#if defined(CONFIG_TZ_ENABLED)
        mbedtls_ssl_conf_verify(&conf, mbedtls_verify_nsc, NULL);
#else
        mbedtls_ssl_conf_verify(&conf, my_verify, NULL);
#endif
    }else{
        mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_NONE );
    }
#else
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_NONE );
#endif
#if TLS_VERIFY_CLI_CERT
    if(need_cert)
        mbedtls_ssl_conf_own_cert(&conf, &cli_cert, &cli_key);
#endif

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        printf( " failed\r\n  ! mbedtls_ssl_setup returned %d\r\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, server_host ) ) != 0 )
    {
        printf( " failed\r\n  ! mbedtls_ssl_set_hostname returned %d\r\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    /*
    * 4. Handshake
    */
    printf( "  . Performing the SSL/TLS handshake..." );

    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            printf( " failed\r\n  ! mbedtls_ssl_handshake returned -0x%x\r\n", -ret );
            goto exit;
        }
    }

    printf( " ok\r\n" );

    /*
    * 5. Verify the server certificate
    */
    printf( "  . Verifying peer X.509 certificate..." );

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 )
    {
        char vrfy_buf[512];

        printf( " failed\r\n" );

        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

        printf( "%s\r\n", vrfy_buf );
    }
    else
        printf( " ok\r\n" );

    if (https_method > 0)
    {
    /*
    * 6. Write the HTTP request
    */
        printf("  > Write to server:");

        switch (https_method)
        {
            case HTTPS_GET:
                len = sprintf((char *)buf, "%s%s%s %s%s", "GET https://",server_host,server_path,HTTP_PROTOCOL,"\r\n");
                break;
            case HTTPS_HEAD:
                len = sprintf((char *)buf, "%s%s%s %s%s", "HEAD https://",server_host,server_path,HTTP_PROTOCOL,"\r\n");
                break;
            case HTTPS_OPTIONS:
                len = sprintf((char *)buf, "%s%s%s %s%s", "OPTIONS https://",server_host,server_path,HTTP_PROTOCOL,"\r\n");
                break;
            case HTTPS_TRACE:
                len = sprintf((char *)buf, "%s%s%s %s%s", "TRACE https://",server_host,server_path,HTTP_PROTOCOL,"\r\n");
                break;
            case HTTPS_POST:
                len = sprintf((char *)buf, "%s%s%s %s%s%u%s%s", "POST https://",server_host,server_path,HTTP_PROTOCOL,"Content-Length:",strlen(https_request_body),"\r\nContent-Type:application/x-www-form-urlencoded\r\n\r\n",https_request_body);
                break;
            default:
                len = sprintf((char *)buf, "%s%s%s %s%s", "HEAD https://",server_host,server_path,HTTP_PROTOCOL,"\r\n");
                break;
        }

        while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                printf(" failed\r\n  ! mbedtls_ssl_write returned %d\r\n", ret);
                goto exit;
            }
        }

        len = ret;
        printf(" %d bytes written\r\n%s\r\n", len, (char *)buf);

    /*
    * 7. Read the HTTP response
    */
#if 1
        printf("  < Read from server:\r\n");
        do
        {
            len = sizeof(buf) - 1;
            memset(buf, 0, sizeof(buf));
            ret = mbedtls_ssl_read(&ssl, buf, len);

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
                break;

            if (ret < 0)
            {
                printf("failed\r\n  ! mbedtls_ssl_read returned %d\r\n", ret);
                break;
            }

            if (ret == 0)
            {
                printf("\r\nEOF\r\n");
                break;
            }

            len = ret;
            printf(" %d bytes read\r\n%s", len, (char *)buf);
        } while (1);
#else
        if (wait_srv_rsp)
        {
            printf("  < Read from server:\r\n");
            len = sizeof(buf) - 1;
            memset(buf, 0, sizeof(buf));
            ret = mbedtls_ssl_read(&ssl, buf, len);
            if (ret < 0)
            {
                printf("\r\nfailed\r\n  ! mbedtls_ssl_read returned %d\r\n", ret);
            }
            else if (ret == 0)
            {
                printf("\r\nEOF\r\n");
            }
            else
            {
                printf("\r\n %d bytes read\r\n%s\r\n", ret, (char *)buf);
            }
        }
#endif
    }
    mbedtls_ssl_close_notify( &ssl );

exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        printf("Last error was: %d - %s\r\n", ret, error_buf );
    }
#endif
    mbedtls_net_free( &server_fd );
#if TLS_VERIFY_SRV_CERT
    mbedtls_x509_crt_free( &ca_cert );
#endif
#if TLS_VERIFY_CLI_CERT
    mbedtls_x509_crt_free( &cli_cert );
    mbedtls_pk_free( &cli_key );
#endif
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );

    printf("\r\nExit ssl client task: stack high water mark = %d\r\n", sys_stack_free_get(NULL));

    sys_task_delete(NULL);
}


#define MAX_CIPHERSUITE_SET_NUM       8
#define MAX_CIPHERSUITE_SET_MEMBER    8
static int ciphersuite_sets[MAX_CIPHERSUITE_SET_NUM][MAX_CIPHERSUITE_SET_MEMBER]=
{
    /* RSA */
    {
        MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA,     //0x2F
        MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,  //0x3C   /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256,  //0x3D   /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256,  //0x9C   /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_256_GCM_SHA384,  //0x9D   /**< TLS 1.2 */
    },
    /* ECDH */
    {
        MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256, //0xC025 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384, //0xC026 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,   //0xC029 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,   //0xC02A /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256, //0xC02D /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384, //0xC02E /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,   //0xC031 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,   //0xC032 /**< TLS 1.2 */
    },
    /* ECDHE */
    {
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, //0xC023 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, //0xC024 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,   //0xC027 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,   //0xC028 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, //0xC02B /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, //0xC02C /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,   //0xC02F /**< TLS 1.2 */
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,   //0xC030 /**< TLS 1.2 */
    },
    /* CAMELLIA */
    {
        MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256,        //0xC07A /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384,        //0xC07B /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256,    //0xC07C /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384,    //0xC07D /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256, //0xC088 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384, //0xC089 /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256,   //0xC08C /**< TLS 1.2 */
        MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384,   //0xC08D /**< TLS 1.2 */
    },
    /* CCM */
    {
        MBEDTLS_TLS_RSA_WITH_AES_128_CCM,        //0xC09C /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_256_CCM,        //0xC09D /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CCM,    //0xC09E /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CCM,    //0xC09F /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_128_CCM_8,      //0xC0A0 /**< TLS 1.2 */
        MBEDTLS_TLS_RSA_WITH_AES_256_CCM_8,      //0xC0A1 /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CCM_8,  //0xC0A2 /**< TLS 1.2 */
        MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CCM_8,  //0xC0A3 /**< TLS 1.2 */
    },
    /* PSK */
    {
        MBEDTLS_TLS_PSK_WITH_RC4_128_SHA,          //0x8A
        MBEDTLS_TLS_PSK_WITH_3DES_EDE_CBC_SHA,     //0x8B
        MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA,      //0x8C
        MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA,      //0x8D
        MBEDTLS_TLS_RSA_PSK_WITH_RC4_128_SHA,      //0x92
        MBEDTLS_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA, //0x93
        MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA,  //0x94
        MBEDTLS_TLS_RSA_PSK_WITH_AES_256_CBC_SHA,  //0x95
    },
    /* 3DES */
    {
        MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA,         //0x0A
        MBEDTLS_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA,     //0x16
        MBEDTLS_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA,     //0x8F
        MBEDTLS_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA,     //0x93
        MBEDTLS_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,  //0xC003
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA, //0xC008
        MBEDTLS_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA,    //0xC00D
        MBEDTLS_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,   //0xC012
    },
    /* RC4 */
    {
        MBEDTLS_TLS_DHE_PSK_WITH_RC4_128_SHA,         //0x8E
        MBEDTLS_TLS_RSA_PSK_WITH_RC4_128_SHA,         //0x92
        MBEDTLS_TLS_ECDH_ECDSA_WITH_RC4_128_SHA,      //0xC002
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA,     //0xC007
        MBEDTLS_TLS_ECDH_RSA_WITH_RC4_128_SHA,        //0xC00C
        MBEDTLS_TLS_ECDHE_RSA_WITH_RC4_128_SHA,       //0xC00C
    },
};
unsigned int ciphersuite_pref_count;
extern int ciphersuite_preference_array[MAX_CIPHERSUITE_PREFERENCE_ARRAY_LEN];
extern const int ciphersuite_preference_default[];
/*!
    \brief      initialize mbedtls ciphersuite preference
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mbedtls_cs_pref_init(void)
{
    ciphersuite_pref_count = MAX_CIPHERSUITE_PREFERENCE_ARRAY_LEN;
    memcpy((void *)ciphersuite_preference_array, ciphersuite_preference_default, MAX_CIPHERSUITE_PREFERENCE_ARRAY_LEN);
}
/*!
    \brief      clear mbedtls ciphersuite preference
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mbedtls_cs_pref_clear(void)
{
    ciphersuite_pref_count = 0;
    memset((void *)ciphersuite_preference_array, 0, sizeof(ciphersuite_preference_array));
}
/*!
    \brief      add mbedtls ciphersuite preference
    \param[in]  id: id want to add
    \param[out] none
    \retval     none
*/
static void mbedtls_cs_pref_add(int id)
{
    ciphersuite_preference_array[ciphersuite_pref_count++] = id;
}
/*!
    \brief      dump mbedtls ciphersuite preference
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mbedtls_cs_pref_dump(void)
{
    int i = 0, print_n = ciphersuite_pref_count;
    printf("\r\nCiphersuite Preference(%d):", ciphersuite_pref_count);
    if(print_n > 10)
        print_n = 10;
    for(i=0; i<print_n; i++)
    {
        printf("\r\n\t0x%x - %s", ciphersuite_preference_array[i], mbedtls_ssl_get_ciphersuite_name(ciphersuite_preference_array[i]));
    }
    printf("\r\n");
}

/*!
    \brief      set cipher suite parse
    \param[in]  arg: pointer to the input value
    \param[out] none
    \retval     none
*/
void parse_cipher_suite_set(char *arg)
{
    int setid = atoi(arg);
    int i = 0;

    if(setid < MAX_CIPHERSUITE_SET_NUM)
    {
        mbedtls_cs_pref_clear();
        for(i=0; i<MAX_CIPHERSUITE_SET_MEMBER; i++)
        {
            if(ciphersuite_sets[setid][i] != 0)
                mbedtls_cs_pref_add(ciphersuite_sets[setid][i]);
        }
    }
}

/*!
    \brief      parse cipher suite
    \param[in]  arg: pointer to the input value
    \param[out] none
    \retval     none
*/
static void cipher_suites_parse(char *arg)
{
    char buf[64];
    char *p, *q, *endptr;
    uint32_t id;

    if(strlen(arg) > sizeof(buf)){
        printf("\rThe input ciphersuite string is too long(%d).\r\n", strlen(arg));
        return;
    }
    mbedtls_cs_pref_clear();
    strcpy(buf, arg);
    q = buf;
    while(*q != '\0'){
        p = q;
        while((*q != ',')&&(*q != '\0')) q++;
        if(*q == ','){
            *q = '\0';
            q++;
        }
        id = (uint32_t)strtoul(p, &endptr, 16);
        if(((id & 0xFFFFFF00) == 0xC000) || ((id & 0xFFFFFF00) == 0x00))
            mbedtls_cs_pref_add(id);
    };
}

#ifdef TLS_CRT_USED
/*!
    \brief      parse certificates type
    \param[in]  arg: pointer to the input value
    \param[out] none
    \retval     none
*/
static int cert_type_parse(char *arg)
{
    if(strcmp(arg, "rsa1") == 0){
        cert_type = TLS_CRT_1_RSA_1024_SHA256;
    }else if(strcmp(arg, "rsa2") == 0){
        cert_type = TLS_CRT_1_RSA_2048_SHA1;
    }else if(strcmp(arg, "rsa3") == 0){
        cert_type = TLS_CRT_1_RSA_3072_SHA256;
    }else if(strcmp(arg, "ecp1") == 0){
        cert_type = TLS_CRT_1_ECDSA_PRIME256V1_SHA256;
    }else if(strcmp(arg, "ecp2") == 0){
        cert_type = TLS_CRT_1_ECDSA_SECP384R1_SHA384;
    }else if(strcmp(arg, "ecp3") == 0){
        cert_type = TLS_CRT_1_ECDSA_BRAINP512R1_SHA512;
    }else if(strcmp(arg, "ecp4") == 0){
        cert_type = TLS_CRT_1_ECDSA_SECP521R1_SHA512;
    }else if(strcmp(arg, "rsa_chain") == 0){
        cert_type = TLS_CRT_3_RSA_2048_SHA512;
    }else if(strcmp(arg, "ecp_chain") == 0){
        cert_type = TLS_CRT_3_ECDSA_SECP521R1_SHA512;
    }else if(strcmp(arg, "baidu") == 0){
        cert_type = BAIDU_CERT;
    }else{
        printf("\rThe input cert type is unknown(%s).\r\n", arg);
        return -1;
    }
    //printf("\r\n%s(): arg = %s cert_type = %d\r\n", __func__, arg, cert_type);
    return 0;
}
#endif

static void parse_request_method(char *arg)
{
    if(strcmp(arg, "get") == 0){
        https_method = HTTPS_GET;
    }else if(strcmp(arg, "head") == 0){
        https_method = HTTPS_HEAD;
    }else if(strcmp(arg, "options") == 0){
        https_method = HTTPS_OPTIONS;
    }else if(strcmp(arg, "trace") == 0){
        https_method = HTTPS_TRACE;
    }else if(strcmp(arg, "post") == 0){
        https_method = HTTPS_POST;
    }else{
        printf("\rThe input request method is unknown(%s).\r\n", arg);
    }
}

/* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_SSL_TLS_C &&
          MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
          MBEDTLS_CERTS_C && MBEDTLS_PEM_PARSE_C && MBEDTLS_CTR_DRBG_C &&
          MBEDTLS_X509_CRT_PARSE_C */
/*!
    \brief      command of ssl client
    \param[in]  argc: counter of input argument
    \param[out] argv: pointer to the input argument
    \retval     none
*/
void cmd_ssl_client(int argc, char **argv)
{
    int arg_cnt = 2;
#ifdef TLS_CRT_USED
    int ret = 0;
    cert_type = TLS_CRT_NONE;
#endif
    debug_level = 1;
    wait_srv_rsp = 1;
    https_method = 0;
    memset(server_path,0,sizeof(server_path));
    memset(https_request_body,0,sizeof(https_request_body));
    strcpy(server_host, SERVER_NAME);
    strcpy(server_port, SERVER_PORT);

    //cmd_mem_status(0, NULL);

    if(argc < arg_cnt){
        goto PrintHelp;
    }

    while (arg_cnt <= argc) {
        if (strcmp(argv[arg_cnt-1], "-h") == 0) {
            if (argc > arg_cnt)
                snprintf(server_host, sizeof(server_host), "%s", argv[arg_cnt]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-p") == 0) {
            if (argc > arg_cnt)
                snprintf(server_port, sizeof(server_port), "%s", argv[arg_cnt]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-cs") == 0) {
            if (argc > arg_cnt)
                cipher_suites_parse(argv[arg_cnt]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-ss") == 0) {
            if (argc > arg_cnt)
                parse_cipher_suite_set(argv[arg_cnt]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-cert") == 0) {
#ifdef TLS_CRT_USED
            if (argc > arg_cnt)
                ret = cert_type_parse(argv[arg_cnt]);
            if (ret < 0)
                goto PrintHelp;
#endif
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-default") == 0) {
            mbedtls_cs_pref_init();
            arg_cnt += 1;
        } else if (strcmp(argv[arg_cnt-1], "-debug") == 0) {
            if (argc > arg_cnt)
                debug_level = atoi(argv[arg_cnt]);
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt-1], "-norsp") == 0) {
            wait_srv_rsp = 0;
            arg_cnt += 1;
        }  else if (strcmp(argv[arg_cnt-1], "-path") == 0) {
            if (argc > arg_cnt)
                snprintf(server_path, sizeof(server_path), "%s", argv[arg_cnt]);
            arg_cnt += 2;
        }  else if (strcmp(argv[arg_cnt-1], "-method") == 0) {
            if (argc > arg_cnt)
                parse_request_method(argv[arg_cnt]);
            arg_cnt += 2;
        }  else if (strcmp(argv[arg_cnt-1], "-postdata") == 0) {
            if (argc > arg_cnt)
                snprintf(https_request_body, sizeof(https_request_body), "%s", argv[arg_cnt]);
            arg_cnt += 2;
        }  else {
            printf("\r\nFormat error.");
            goto PrintHelp;
        }
    }

    printf("Server Host: %s Port: %s\r\n", server_host, server_port);
    mbedtls_cs_pref_dump();

    if (NULL == sys_task_create(NULL, (const uint8_t *)"ssl_client", NULL,
                    SSL_CLIENT_TASK_STK_SIZE, 0, SSL_CLIENT_TASK_PRIO,
                    (task_func_t)ssl_client, NULL)) {
        printf("ERROR: Create ssl client task failed\r\n");
    }

    return;

PrintHelp:
    printf("Usage: %s [-h Host] [-p Port] [-cs CiperSuite] [-ss cipherSuiteSet] [-cert CertType] [-path Path] [-method Method] [-postdata Postdata]\r\n", argv[0]);
    printf("Example:\r\n");
    printf("\r\n\t\t%s -h www.baidu.com", argv[0]);
    printf("\r\n\t\t%s -h 192.168.3.100 -p 4433", argv[0]);
    printf("\r\n\t\t%s -h www.baidu.com -cs c02f", argv[0]);
    printf("\r\n\t\t%s -h www.baidu.com -cs 3c,3d", argv[0]);
    printf("\r\n\t\t%s -h www.baidu.com -ss 0", argv[0]);
    printf("\r\n\t\t%s -default", argv[0]);
    printf("\r\n\t\t%s -h 192.168.3.100 -p 4433 -cert rsa1", argv[0]);
    printf("\r\n\t\t%s -h 192.168.3.100 -p 4433 -cert ecp_chain", argv[0]);
    printf("\r\n\t\t%s -h passport.jd.com -p 443 -method post -path /new/login.aspx -postdata username=werty&password=erfgss", argv[0]);
    printf("\rOption:\r\n");
    printf("\r\n\t\t-h host: server host name or ip");
    printf("\r\n\t\t-p port: server port");
    printf("\r\n\t\t-cs cipersuite: ciphersuite number");
    printf("\r\n\t\t\t\t3C - MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256");
    printf("\r\n\t\t-ss cipersuiteset: ciphersuite set number 0 - 7");
    printf("\r\n\t\t\t\t0 - MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256");
    printf("\r\n\t\t\t\t  - MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256");
    printf("\r\n\t\t\t\t  - MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA");
    printf("\r\n\t\t-cert type: type is choosed from {rsa1, rsa2, rsa3, ecp1, ecp2, ecp3, ecp4, rsa_chain, ecp_chain}");
    printf("\r\n\t\t\t\trsa1 - TLS_CRT_1_RSA_1024_SHA256");
    printf("\r\n\t\t\t\trsa2 - TLS_CRT_1_RSA_2048_SHA1");
    printf("\r\n\t\t\t\trsa3 - TLS_CRT_1_RSA_3072_SHA256");
    printf("\r\n\t\t\t\tecp1 - TLS_CRT_1_ECDSA_PRIME256V1_SHA256");
    printf("\r\n\t\t\t\tecp2 - TLS_CRT_1_ECDSA_SECP384R1_SHA384");
    printf("\r\n\t\t\t\tecp3 - TLS_CRT_1_ECDSA_BRAINP512R1_SHA512");
    printf("\r\n\t\t\t\tecp4 - TLS_CRT_1_ECDSA_SECP521R1_SHA512");
    printf("\r\n\t\t\t\trsa_chain - TLS_CRT_3_RSA_2048_SHA512");
    printf("\r\n\t\t\t\tecp_chain - TLS_CRT_3_ECDSA_SECP521R1_SHA512");
    printf("\r\n\t\t-path path: path of url");
    printf("\r\n\t\t-method method: method of http request: head, get, options, trace, post");
    printf("\r\n\t\t\t\tif method is post, must use -postdata option");
    printf("\r\n\t\t-postdata postdata: request data of http request, only use when http request method is post");
    printf("\r\n");
    return;
}

#endif //CONFIG_SSL_TEST
