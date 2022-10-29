/*!
    \file    ssl_certs.c
    \brief   SSL certificate for GD32W51x WiFi SDK

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

#if defined(TLS_CRT_USED)
static const char rsa1_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICHTCCAYYCCQCulg/OwFrsFzANBgkqhkiG9w0BAQsFADBTMQswCQYDVQQGEwJj\r\n" \
"bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDg0NDQ1WhcNMjcxMjI1\r\n" \
"MDg0NDQ1WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UE\r\n" \
"BwwGc3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZ8w\r\n" \
"DQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMujn/SmDjS33/gpWq7oe4c9Z1HPnhDX\r\n" \
"yQnbwHJwDPWOS/KcBV0TrA5qfRea0As5cfXBxjefEHeItuSR81Te3AqrK7/yWsAe\r\n" \
"OOLwfmSiY6sAdnsflcaM0uhxbt7rirFTvQLcTMADaP3C/ykB6hAD8/6XFDZz9BuC\r\n" \
"pYvmo9/wp3GJAgMBAAEwDQYJKoZIhvcNAQELBQADgYEARyBWJs+StQgr4vOmCOiG\r\n" \
"saIA5od0bM149frX2LV/PEBusiSGC3JH4Qe5LY7oIfKKxFVt7ZQLAFzTWEFvoiSZ\r\n" \
"/3rGrFGeMn4UO2/ImyN35Ld4KjnxJzXXn42q+uQLVv39T6Vok5fofAJDH/18xYxg\r\n" \
"OQmCW9y9TMbeZZ10zLdqsQo=\r\n" \
"-----END CERTIFICATE-----\r\n";

static const char rsa1_cli_key[]=
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIICXQIBAAKBgQDF1y6cWqlmASkxTUue2obcck3burDnDvBn5AplEZAdfAFzMndt\r\n" \
"Au/zLyI05ujjYw1N0W1TNrdJY5XxkWpvDomacPtEnK+274OC58Q7HiEAh1SxeNgf\r\n" \
"q4pvQ3esUwVu6Ls/vSFpfjpFeyKsk1ucXEfENdCEh+b+K/qkk7zF9AEBfwIDAQAB\r\n" \
"AoGBAJMwUpc0xE8FkhYCAb6/qhIcYFyXesGM1cMVX75t4KBu/80qwLszsj1k1bgy\r\n" \
"CxYRPXal1wZP8PECzC2bGGpjkG8tma19vFbIXOinJdiNj0HpqyR7uWJORZC26fYM\r\n" \
"tX8MNEzqkV3SLaBRiQ8nElQy/IkSwpHzrBsO9TgN3GetjIuxAkEA5575sh2c3TQ5\r\n" \
"hF/0xxw1HW4p+cZaiaBgLFypkk1mXyTUaFX9d2frz8Oe/pac4sR9lBnYmTyTRg/v\r\n" \
"TfCGjYVNkwJBANqp/j4C4362JceT3bvROkw1hrxaX2mivhhBzmnA2Ebz4aEPjKUH\r\n" \
"vpOPBGx4UxthIHmvrJ/DFzjJuuqbK01ND+UCQEzSrM0IB2RTExS14vE7iN53EJMY\r\n" \
"2CS3vc5Y+aFd7Kt4Ar+MbeJx5IPnxU950xVfyKsbm3zP26UsWdoHAgnkgeMCQEM6\r\n" \
"/Ran4LZ23orMZeJ3ZAtGcdS7nJZoGTZwFTzitByso3TXyRB8nxXTZTLMlBDY/hkr\r\n" \
"8FF2tE8bh0LWzquHxBkCQQCHH37ie0ErnR9+71JkY4hM8qbo7plOkSDBoyZ/xsxg\r\n" \
"1BylXma0s48nwJeYAOSvxVVw5oupoZgxzvL4oPHRzZgJ\r\n" \
"-----END RSA PRIVATE KEY-----";

static const char rsa1_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICHTCCAYYCCQDcr9nMMGEhyzANBgkqhkiG9w0BAQsFADBTMQswCQYDVQQGEwJj\r\n" \
"bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDg0NTAwWhcNMjcxMjI1\r\n" \
"MDg0NTAwWjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UE\r\n" \
"BwwGc3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZ8w\r\n" \
"DQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMXXLpxaqWYBKTFNS57ahtxyTdu6sOcO\r\n" \
"8GfkCmURkB18AXMyd20C7/MvIjTm6ONjDU3RbVM2t0ljlfGRam8OiZpw+0Scr7bv\r\n" \
"g4LnxDseIQCHVLF42B+rim9Dd6xTBW7ouz+9IWl+OkV7IqyTW5xcR8Q10ISH5v4r\r\n" \
"+qSTvMX0AQF/AgMBAAEwDQYJKoZIhvcNAQELBQADgYEAZ/vXyB7vmQodNWKMDIfq\r\n" \
"ZBpAyOnlWoh66eSVVp0CKH8+XwCI2KNbMnztAuvwOFxfjjvmXkcEIgR425hTq0n2\r\n" \
"bAudp8yTi7bx7pNQpnUveoQqf2gPjvWttkBsmdmUDF40q0OLA9meYGD8ZrMxwaV4\r\n" \
"2Tc+Zfb2TdIxgunYpj5F5E8=\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char rsa2_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDIjCCAgoCCQC/dO9fPW7UzzANBgkqhkiG9w0BAQUFADBTMQswCQYDVQQGEwJj\r\n" \
"bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDM0NjEwWhcNMjcxMjI1\r\n" \
"MDM0NjEwWjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UE\r\n" \
"BwwGc3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEi\r\n" \
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCXPONjXaAifR+nlyp4N2vAihfe\r\n" \
"h4Ud59t3yKh8mfctvpRlOfp9IV+bq/sHenmh7NJEvH/GDDuRQ6U+9TEKrjutexEV\r\n" \
"U1ra9gd2dwtbWxB14g9ufUpV3o1e5sgiYI7Xeq33qmq5/CCOHyqjZUBLF/ZOZDIs\r\n" \
"814ok15dg3XKQeu80ZUmneRCqYJCJpRiJBH/vFC0SF9nvJRhEb05vTwS+1EybsRh\r\n" \
"wuqC5FSdwmdr/np923nW3au8Y2VHQ2wtg++su53XpqZs7eR1zOFzjFZhK3Wi5NKu\r\n" \
"fxfeagU8yL6l/UOEKAgRozEAhcKPces6Rpn4/n1g7antxRKiD0I7EZoO773vAgMB\r\n" \
"AAEwDQYJKoZIhvcNAQEFBQADggEBADMcyrYNE74zPPtQkXYDEs4O9v3ryPh/Exvy\r\n" \
"qKLXTtROzl60LS04HyYgqOihzfqLBqgRZccDobbeU0IZsKE+0tepJ1hVwlHB9XN8\r\n" \
"LyQU+iS/V+dIOLNzYIqsUZ9LZeI8FTg7kQQn/mMhYEqXSex33zB8EouYX4TWT7Je\r\n" \
"Yu8AEcIf/sJsxr82E6HGB4o0VqeIdYpgaEaC6fJUWex2kYAr8v8AOc9yrc6Ab1dR\r\n" \
"F7vOLsDvKJ+0Vb/eUZgJCqYyrfr44risyS6nmJZ5i48tRyWT4dVbqWCgLTAnhbPm\r\n" \
"oOQXw+hHJpUqMNP257S0YbvLCmdontSry2eoC93WWY9w6AkcVxw=\r\n" \
"-----END CERTIFICATE-----";

static const char rsa2_cli_key[]=
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIIEpQIBAAKCAQEAyT8MesjzxztLBqxz05QVZv47KuNkZUDdX6qbXfAwskjfgotg\r\n" \
"JefIrRAFl1hKUqyGGfK/P0zmk/1iQ3ZaLRIUKfHI+IsR7Pz00TqaxsXxPxLI+E3S\r\n" \
"qPbNY5poIf1FiDXS6rU46B0r+GxPOS0ql+7/hO2Rl21rljXTq0JLp24GCT6RRJd8\r\n" \
"A2uSIBmZpeFcORzwsE2Y4KXQd7B14/yskzTc6uOZlQiLSC9n2AdGzlNZxNB7I/V6\r\n" \
"mptm8VkYOZSV9rmNTx0haw7YpNidiTC3EAbEZ/8iEB+UEIa0EyI8T1mFq6FqM8H0\r\n" \
"EIBImXAc1bvOAyZ4xaaCixAeZpvey/JE9gmwIQIDAQABAoIBAQC7XEarHeHQMAS2\r\n" \
"ruGbLvgKDlFZqaDzn8M6aOUDPTapFa15pXcbc0Li1NLPx7RSairjh5vEPD1DtJjC\r\n" \
"hDKKWZWKQ3++3gMPZ6YqEWy9+NWUylFu0014X2Zy//NJCbqtiKbYO2ZCu74PKA9O\r\n" \
"BR3tm+YqhN+SYp3SmER74ln7BDT8bqpXBzDak09FjiHYfxjv3YSlO/a5sklC01ah\r\n" \
"QHIMYS1eB+Pg6lbqvaDs/flnJfwXRESHQHnz6oh4nVAHrIsI+iLzCNf5kkrLlmFM\r\n" \
"3r0e+HzbrUZ4Fm6VTT5crk3596e45/CyucMdqqIyd1g0cezqiXaOoEKONEzXYXTi\r\n" \
"x3sY0JgBAoGBAPmTArO9Ij5J2SxSvYCJAKaz2vqGoTFLLn4XIhcsrgMz6IO5xdJ9\r\n" \
"jiLf6Mw0XGLkdL7MNAoUYJnWhONO17C5dHlN9tAbVGsqf9JzbbmpEN+Fve41XUDW\r\n" \
"UX6clXEMbF6oTojhcBOuYBMKLKLw2tlzjMDDFkNfmC/Dficpxm20ImnBAoGBAM5t\r\n" \
"f+v+Uj+HxQR+KbSDT6vl1JV5mEKHQxsyX1ekA34sHoKNA6lM+rbq1xIg1E6VAJOq\r\n" \
"5+EtTM2jFKZY87jFb59sHqGOc+JMi8RniAPpMkVRHNaAy6dMKocsqTzuDrkxUX0l\r\n" \
"jMWlhbDhAHFoNRxmuXaF+BKRUtrMDm5p2/9VWx5hAoGBAIHSykYLLYPvUEwxIih5\r\n" \
"m+AbkN5fJ/yHWkD28aHSeRZraxkSiugKreRQSz3mQuSiaGRTvwI7RWDg6TGA8swY\r\n" \
"PJFZT/lDLi+s5N7pcntwiblsTbdbBQ5ULgJJm6c5yimXLXsgofdq7skYss1zsrQI\r\n" \
"2n/vO2aClJzL+ZSPs4ufd4NBAoGAT9mtnRUwraCQRo+jBE+zFvR9iiCSFFIMVE3k\r\n" \
"8ZrhOwJH/wobO3cqmG05KqsGVStwIK8AqWsDhZwq1boc9QFyj0Gci7P2AbjIa/38\r\n" \
"cD7ZklSxCgyiOqHM5aMSCB6yFVMNiZJn5OCFWYl/yV6dBNJ7tWxXLJ+IZFiA0Jck\r\n" \
"YZ2b9KECgYEAj/W+YxdVw/8J4iSsmdmvPaH11c7rYuKg3fw5l0BPmlQItURnbr5O\r\n" \
"TGroRmDSHu2HnUbEleLqbhGQVnAR6eq8BMzy0BsKWzBo6LlHzO23qmVn/g8Rl/Zq\r\n" \
"cmmYCFitnX9fLnI3lV61TsohFY4d2x+GCrpz3RhE/kEPN88L5VZSnuA=\r\n" \
"-----END RSA PRIVATE KEY-----";

static const char rsa2_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDDjCCAfYCCQD2Zw01CjRzNDANBgkqhkiG9w0BAQUFADBTMQswCQYDVQQGEwJj\r\n" \
"bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDM0NzU4WhcNMjcxMjI1\r\n" \
"MDM0NzU4WjA/MQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UE\r\n" \
"BwwGc3V6aG91MQ0wCwYDVQQKDARyc2EyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\r\n" \
"MIIBCgKCAQEAyT8MesjzxztLBqxz05QVZv47KuNkZUDdX6qbXfAwskjfgotgJefI\r\n" \
"rRAFl1hKUqyGGfK/P0zmk/1iQ3ZaLRIUKfHI+IsR7Pz00TqaxsXxPxLI+E3SqPbN\r\n" \
"Y5poIf1FiDXS6rU46B0r+GxPOS0ql+7/hO2Rl21rljXTq0JLp24GCT6RRJd8A2uS\r\n" \
"IBmZpeFcORzwsE2Y4KXQd7B14/yskzTc6uOZlQiLSC9n2AdGzlNZxNB7I/V6mptm\r\n" \
"8VkYOZSV9rmNTx0haw7YpNidiTC3EAbEZ/8iEB+UEIa0EyI8T1mFq6FqM8H0EIBI\r\n" \
"mXAc1bvOAyZ4xaaCixAeZpvey/JE9gmwIQIDAQABMA0GCSqGSIb3DQEBBQUAA4IB\r\n" \
"AQACsHeSfrhEozF09byWZyXWJTiRaKaAjChPKfK8+ri1fXv98ksvMdYkI2jX/uFp\r\n" \
"1+BKgZ1jbX4NHCMUMiu4JweOs7nhVyFNlpDMYBsd2TPVNMb3iJO65r3DNq5nN1/Z\r\n" \
"UmSPsA8p5UD5JGeWiJrj+Izoo6LNXkuXHOev+wJYYeiAEaAUNshBHvxdHrpeunWn\r\n" \
"ZBViQQ0qOhdsjcL/tVFwKyT0oMcaiQ6e7o33uXfGHhIZg5IH/lYhAHEvBSx2LKZF\r\n" \
"8M3IFUvhmLbM6fItJQSWSdf4LZdwm+JEByp62jN0zx7xcUgeC54Bzyvx2EuM/jZ0\r\n" \
"iqhx+s03//r+pnEtn9R2kfq9\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char rsa3_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIEuDCCAyACCQCgEIoE8iI//jANBgkqhkiG9w0BAQsFADCBnTELMAkGA1UEBhMC\r\n" \
"Y24xEDAOBgNVBAgMB2ppYW5nc3UxDzANBgNVBAcMBnN1emhvdTEhMB8GA1UECgwY\r\n" \
"SW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQswCQYDVQQLDAJzczEWMBQGA1UEAwwN\r\n" \
"MTkyLjE2OC4zLjEwMDEjMCEGCSqGSIb3DQEJARYUbGVvLmxpQGdpZ2FkZXZpY2Uu\r\n" \
"Y24wHhcNMjAwODAzMDkxNjExWhcNMzAwODAxMDkxNjExWjCBnTELMAkGA1UEBhMC\r\n" \
"Y24xEDAOBgNVBAgMB2ppYW5nc3UxDzANBgNVBAcMBnN1emhvdTEhMB8GA1UECgwY\r\n" \
"SW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQswCQYDVQQLDAJzczEWMBQGA1UEAwwN\r\n" \
"MTkyLjE2OC4zLjEwMDEjMCEGCSqGSIb3DQEJARYUbGVvLmxpQGdpZ2FkZXZpY2Uu\r\n" \
"Y24wggGiMA0GCSqGSIb3DQEBAQUAA4IBjwAwggGKAoIBgQDH1q1cCtcQCHf/eOjA\r\n" \
"SDmMMoiBQpiC4oC0iWxUOq+NLjtdcj6IPGwE/QTCkLRFNi5DIJoJ5Yj7JiJYGC0G\r\n" \
"gwbmeFUyCPbTH/Msvf9Vg8xKcGd0WyzZjlKkLvq/o2/+Fzs0v+Cnzi0/RrbBK0In\r\n" \
"7AH2fl3LZDDnBsM+vv0fuuy0TpNzSXFYr4nxagpRPZGwcxur7CoL0N6mtRUDyYap\r\n" \
"2SkujWznac194bno1gucb0hiZti6suqubHBjf7zcSAkYWLmCWWzK+M8of3ohufKT\r\n" \
"3pR6n4joZhdtCp3hQrlwNqNPMxA4I6tf/K1nPNuDcx32EHVXIxkBPxWbAX02LGDk\r\n" \
"Z7t1BFt3MGO8fDf4jOQSI3BARBe3EL2iaKoF1FZ1VyH6i8dEJSHYvYh1NcZN04Sb\r\n" \
"J21Fs+K3OHsks2010xwsKN4v9g7C5RnujqWSox2CeWbIX/mpXhB8lQzA9niMFnfs\r\n" \
"Uxc8pPdrAgFwybla9yKFmvoL33SKBRWSb9G8FS2U9IjmxMkCAwEAATANBgkqhkiG\r\n" \
"9w0BAQsFAAOCAYEAKUHvTrZqrZvzU0rD99BFKEnzAifdL32E3MqUdKEnyug3wvmK\r\n" \
"OIBix+d38rpFjm78DJ5nri56/zagTUcdM/Vvve2ecdQFZgorXuM031y9R8HkFzCc\r\n" \
"w8001UE7xpaEA2Qki4dfdmDlaMKMiLPSHYL4Bfbz5jssIzEAEAa6/2tqB1xt8jlV\r\n" \
"Klo1njBzyEu3ilQ151uRD7ECn4Uy2/ALGfs5BxTSSbOqi24+4nnKUeuXcGmWkLCf\r\n" \
"M/aet0wz+r1c0JlKE5Ws8fxwKoCda6lShs8RTUEVAiKlI6UUKKS8xXDzssrZJTdm\r\n" \
"fD+HdxIpEcVnT7l7o1BCIM6QN5ZSvdnSfMwDT6RqGHV7frX/antHyHX/hHApinXs\r\n" \
"xD6ftuZX9f/MAjtTgZKazRC/gu1xpnYbyBoe8dA/u+GaT3tQ52Jfqrg65RNcsFOq\r\n" \
"hZ1sGMd6hTPjJ98U19cSRMie7UHBUM8Veymb0Uox24z0aLK52F6B0bc/KBqrwTve\r\n" \
"UQ8w9z/cdw/QFPxO\r\n" \
"-----END CERTIFICATE-----";

static const char rsa3_cli_key[]=
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"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\r\n" \
"-----END RSA PRIVATE KEY-----";

static const char rsa3_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIEejCCAuICCQDgOOqS8MSgXTANBgkqhkiG9w0BAQsFADCBnTELMAkGA1UEBhMC\r\n" \
"Y24xEDAOBgNVBAgMB2ppYW5nc3UxDzANBgNVBAcMBnN1emhvdTEhMB8GA1UECgwY\r\n" \
"SW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQswCQYDVQQLDAJzczEWMBQGA1UEAwwN\r\n" \
"MTkyLjE2OC4zLjEwMDEjMCEGCSqGSIb3DQEJARYUbGVvLmxpQGdpZ2FkZXZpY2Uu\r\n" \
"Y24wHhcNMjAwODAzMDkyMDU0WhcNMzAwODAxMDkyMDU0WjBgMQswCQYDVQQGEwJj\r\n" \
"bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxCzAJBgNVBAsMAnNzMIIBojANBgkqhkiG\r\n" \
"9w0BAQEFAAOCAY8AMIIBigKCAYEA5Dor7bs3xHn0ocyqKbUL9Z+Wd1rSWf87Psmt\r\n" \
"socK2iDgYhKN+HaiAUQHVaeckDFxKlh6KEgShxNhQeEqlrAWrZQX48isFjpXH4tG\r\n" \
"b7y2D6BrDtA+oIbHII2hK4dpnLonG/2gp3hBIXk2kE7oJ1CpQ4YPQNDgpT8QKK97\r\n" \
"1HfZ33vDaKJEvyLPm6qW5ND0l4kV9Hj/CfXSkvDKGttm8eIX8RkXe9QDuGYp/rKA\r\n" \
"9WaWqnpfkB6Y3jUOVKHqNinLXH3RsRbHhLkGatnr+Mqr0tuglDgTDd8SQ5HVhyDA\r\n" \
"4nnvmByOcGxJCQtvndISlMmVk3I+djDPDJEKsjHW7WbEQK30eokp5/fVYXnyTm0R\r\n" \
"P/HuN8HgO3h1xQUu13IgqkE9UaZnhKRu9GTy9JuqKv73DWFw8c6RHHH3GkonKxj5\r\n" \
"SHDJCfZjS3pCHXFq0VirJ8ISLLbAdt6hwwyEZez3U/xkDm5awmbTzl6sCZL4gkzg\r\n" \
"v3Th5N3waZScacFcNhMGCcMlUwvJAgMBAAEwDQYJKoZIhvcNAQELBQADggGBAGPj\r\n" \
"fRC239U7JfFw2UIQfLPGXDEljxzWtWbzhq3bqBZIV7SmdpcjxoIEAPpg1Bx/AJM+\r\n" \
"WFy5mEM4xjrN0/+oCrJcohXQ9bvHUDpfzJ7zTIbyxy9Z+PChkegD3CLJuDdjitFj\r\n" \
"ti6n5xmOGTOOk/yCza726zYcLGZNYX/BnM3tq3XS5gywwp+WWk9zDLga+yWFvxzh\r\n" \
"wHUZ/u5gDHrYk0MPcFif1zjeLs0McHMthCoWyVZen8mh4NbUo1Ctbn3zw2/v9nnY\r\n" \
"y75RFWhNLs9eS+ycIPTwbfFqkzju8jglZjIBqwzHm/ehFMFvuPhacgynIvju1qCh\r\n" \
"e0F7mYlB0UYLljSCpm1L+FlQH6+Zuok0u7J7xgSJc1Z8Ixoq6fx3172fFtn5rEBv\r\n" \
"wvpx4DVZFnmiI68HNtm+N2/PGypu5YpwbdmfB3MJoCKJUYiZEBFFpKHXdXb5msVL\r\n" \
"T0RAfNXRnWe77AkTI8FTnM+4nXZyUBc5Om9mRcY/L984u9Yqsy4QZy4CE1XA8w==\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char ecp1_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIBlTCCATwCCQCp+7/HiX5ysTAKBggqhkjOPQQDAjBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDEzMjE4WhcNMjcxMjI1MDEz\r\n" \
"MjE4WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcq\r\n" \
"hkjOPQIBBggqhkjOPQMBBwNCAASVfC7qUx5DLO3eByjpO4M5CugnvOTsN0K0jocK\r\n" \
"Ah549iw8939cX1dB0OYokfgDOhjWAB7+nuAjnIkxVLIZ3HLkMAoGCCqGSM49BAMC\r\n" \
"A0cAMEQCIAmF1zNvZs1gQio53GH2ktLMajLG2/NmnxuPQ0v9ImhFAiA4EziAhJxw\r\n" \
"xIVUA9EGzMQ8zSLnfIYz/Fc9sI9gUsMm0w==\r\n" \
"-----END CERTIFICATE-----";

static const char ecp1_cli_key[]=
"-----BEGIN EC PARAMETERS-----\r\n" \
"BggqhkjOPQMBBw==\r\n" \
"-----END EC PARAMETERS-----\r\n" \
"-----BEGIN EC PRIVATE KEY-----\r\n" \
"MHcCAQEEIGyiihoGYwcYwRqO0J1adV8d3XILC0JJgDuFYhZRXqiNoAoGCCqGSM49\r\n" \
"AwEHoUQDQgAEWoKcVzfHTIKAEzGxPpFvbSPowAQdbjYwhd/9ieOWWOl7+9uTjASt\r\n" \
"JGN/s+IAGz6EEBzMLfgcEiOvwql8FQZDKg==\r\n" \
"-----END EC PRIVATE KEY-----";

static const char ecp1_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIBljCCATwCCQDh3Den7sXSJTAKBggqhkjOPQQDAjBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDEzNDA5WhcNMjcxMjI1MDEz\r\n" \
"NDA5WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcq\r\n" \
"hkjOPQIBBggqhkjOPQMBBwNCAARagpxXN8dMgoATMbE+kW9tI+jABB1uNjCF3/2J\r\n" \
"45ZY6Xv725OMBK0kY3+z4gAbPoQQHMwt+BwSI6/CqXwVBkMqMAoGCCqGSM49BAMC\r\n" \
"A0gAMEUCIQCTwJakTKKy6fFtD/Jmgjxc6K6fVMQFl85JAibNWmouAAIgU9lwl7YZ\r\n" \
"xtDu3b5hdtnUmVmwMYRHrvQfJfQ5veUo6ig=\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char ecp2_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIB0jCCAVkCCQDMCVYctlh9EzAKBggqhkjOPQQDAzBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDEzOTM2WhcNMjcxMjI1MDEz\r\n" \
"OTM2WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwdjAQBgcq\r\n" \
"hkjOPQIBBgUrgQQAIgNiAAReDl+ULj0RM8svXjFxoYgw3hPZXRofUc9nt6SFxnnw\r\n" \
"Fm0bYL5u15fQMmaE0yN7iBo7WnxGW1rIfSJef9n/CTTHSLzAOrvWo3K5gcFcc7c+\r\n" \
"y0TMZ8vxZilKQJRKyfqmMZgwCgYIKoZIzj0EAwMDZwAwZAIwJyVxUD/5XwFjL3uR\r\n" \
"mA46Tn7uKAvL2HyKAptDdf4bPjQoSIoBHWn15ckWBCrf9bOWAjBsIfMd5zZ3PUnb\r\n" \
"a8m/2k0XBECd4bq3VJgi6m4OD2xO/MHFpJzn6Eponvz9zX6mG/A=\r\n" \
"-----END CERTIFICATE-----";

static const char ecp2_cli_key[]=
"-----BEGIN EC PARAMETERS-----\r\n" \
"BgUrgQQAIg==\r\n" \
"-----END EC PARAMETERS-----\r\n" \
"-----BEGIN EC PRIVATE KEY-----\r\n" \
"MIGkAgEBBDCPzqn3ITun1ajIhqRfgRXbeyKiZtq6MxJiL/c7HpcUPbl/4gE9PBrq\r\n" \
"YjrqMuPrEkagBwYFK4EEACKhZANiAASuzUkMN2LSRx8A0Wxlcz9oh+ku4WdzYYJz\r\n" \
"9Ms/r8TFUQlUl+SDQbLw2qCEo7pC3W5NRw9gU3+SbHlMzHGg4z8lJalWjcYFj9Rg\r\n" \
"HH5PqcS+Pl7WdmX0dJVbly4jAH0Wj2M=\r\n" \
"-----END EC PRIVATE KEY-----";

static const char ecp2_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIB0zCCAVkCCQCMVIdlBqKY6TAKBggqhkjOPQQDAzBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDE0MDM4WhcNMjcxMjI1MDE0\r\n" \
"MDM4WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwdjAQBgcq\r\n" \
"hkjOPQIBBgUrgQQAIgNiAASuzUkMN2LSRx8A0Wxlcz9oh+ku4WdzYYJz9Ms/r8TF\r\n" \
"UQlUl+SDQbLw2qCEo7pC3W5NRw9gU3+SbHlMzHGg4z8lJalWjcYFj9RgHH5PqcS+\r\n" \
"Pl7WdmX0dJVbly4jAH0Wj2MwCgYIKoZIzj0EAwMDaAAwZQIxANyDJblwcXwUDF+8\r\n" \
"7wNJM2TdtJFSBJXdW/hSQ+2Z/4qYKJ+VFxAew+cWkTehKXZh1AIwA54u5EkPmllS\r\n" \
"WZW83ncR04f2UIG7B656TfsxzrJsOU6fnY8CMQybyIlPWXGxHKCy\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char ecp3_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICGzCCAX8CCQC73WwgTvH3LzAKBggqhkjOPQQDBDBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDE0MTE1WhcNMjcxMjI1MDE0\r\n" \
"MTE1WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZswFAYH\r\n" \
"KoZIzj0CAQYJKyQDAwIIAQENA4GCAAQ7Ep608v1n31JmRfcIP4tOUSTXwgU4SEc2\r\n" \
"/ilvitK3yBIQecaIUeZT3GeRkj5KA7INCO2dWUblVUm8lcjdn9nWGoUYiwLaeQfj\r\n" \
"cKWlp3TUl6Mj9iXLBipkLOfmepAPWO7m8W7qfoKZ6W/BfXWT4ePkWiH37PVBPpjf\r\n" \
"KHosS4hRdjAKBggqhkjOPQQDBAOBiQAwgYUCQE+ql3PRyAh48IdzOde3aZP4RdeB\r\n" \
"8YFYN+6yIMlY3Uao3GjAqR/hf/uIXxZ0svAXwbxNj0orL2pizxQqFm2qAusCQQCG\r\n" \
"VwEyFvym0p79lNtUqXVE+b0HS4/kHg1Lf8e0t4epnWCsJwsaSQIrjzncfffyP65i\r\n" \
"AvmAFGEQo4ygNUU24X7F\r\n" \
"-----END CERTIFICATE-----";

static const char ecp3_cli_key[]=
"-----BEGIN EC PARAMETERS-----\r\n" \
"BgkrJAMDAggBAQ0=\r\n" \
"-----END EC PARAMETERS-----\r\n" \
"-----BEGIN EC PRIVATE KEY-----\r\n" \
"MIHaAgEBBEAw4BbGfV1nepce1IF9n1XYbNBIvdw6nXZup12YLthNDmuMZIBq3z64\r\n" \
"+bCyfFQRyX3Dme2ZOOEP8MHV61Fp+P2+oAsGCSskAwMCCAEBDaGBhQOBggAECjtV\r\n" \
"qzFwI6uDii92g1ZMGiR55rRX+aBbT/8KLDFLed2vEj1wTO3rLPP2ZqydHqxBGkXO\r\n" \
"oUTOgg9HqW24mBm73WLs8Dt6+NV+QOXRRvuBA3Cy8cq+tv7BHtLhzwNWvVgIwv/L\r\n" \
"jAaakNkZjfKdHdgJbl+GnKNq/+fmZnrTr40Vm6g=\r\n" \
"-----END EC PRIVATE KEY-----";

static const char ecp3_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICGzCCAX8CCQCd0XTRd63G1TAKBggqhkjOPQQDBDBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDE0MjA2WhcNMjcxMjI1MDE0\r\n" \
"MjA2WjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZswFAYH\r\n" \
"KoZIzj0CAQYJKyQDAwIIAQENA4GCAAQKO1WrMXAjq4OKL3aDVkwaJHnmtFf5oFtP\r\n" \
"/wosMUt53a8SPXBM7ess8/ZmrJ0erEEaRc6hRM6CD0epbbiYGbvdYuzwO3r41X5A\r\n" \
"5dFG+4EDcLLxyr62/sEe0uHPA1a9WAjC/8uMBpqQ2RmN8p0d2AluX4aco2r/5+Zm\r\n" \
"etOvjRWbqDAKBggqhkjOPQQDBAOBiQAwgYUCQQCMIt+KZ1dsVtMy4f9gzDJO6zgA\r\n" \
"nqPsjn4jHGZ7zxCJsSydDfhxxL0pkk39lCHAl1u0xbZ7s8+M+RyHWXBg+H+wAkBZ\r\n" \
"MiAiET5m/WOennE2KXIs1oT2Ckp+ftG2s9kpLKXlwPhxbgGInMq5+G2Zg0PPTUBr\r\n" \
"T2iNBdm7FonLrIyszeyj\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char ecp4_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICHTCCAX8CCQCw+Kh9GmnjUjAKBggqhkjOPQQDBDBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDE0MjQzWhcNMjcxMjI1MDE0\r\n" \
"MjQzWjBTMQswCQYDVQQGEwJjbjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZswEAYH\r\n" \
"KoZIzj0CAQYFK4EEACMDgYYABAAtgjhbJlqo1ZuS0Bw4Lxe9B1MAaz48UU4LRSCs\r\n" \
"9LDk53EvDw0ulLjsdp59w0HCaiU8mOCS4IL6achfuovSuoXoXgCDsNnjyGGXoSd/\r\n" \
"EP0ckZ3Hh1SlqQtOXs58zg54vZVcSSeB3RN8x7nNgJK9AJuRTsOGhLac5jKNck/B\r\n" \
"/yfs9NfUfTAKBggqhkjOPQQDBAOBiwAwgYcCQgEx6dj8QDsR6xL61TXihzwzts3V\r\n" \
"XdFqGgA5VRYKpZR9BJhAPxvSsE3c/+V/wkLPf/oFclZwb4aHn209LMKl/AMEnQJB\r\n" \
"YtgW7jZU8skpCw4Sx5wWqtjmDMKulnaYWls6OU8QNWGSLH1UUlsgEFwraVIRcrUw\r\n" \
"u1BTwDSESOiYzHy+ez0a8UQ=\r\n" \
"-----END CERTIFICATE-----";

static const char ecp4_cli_key[]=
"-----BEGIN EC PARAMETERS-----\r\n" \
"BgUrgQQAIw==\r\n" \
"-----END EC PARAMETERS-----\r\n" \
"-----BEGIN EC PRIVATE KEY-----\r\n" \
"MIHcAgEBBEIBRfLAKIC/LDJYjmyyqhxvbiTsSpOsZK403IpXwLUpjSTVnPSTGsLJ\r\n" \
"ZBpMHjpWD/IAIHk1E8Ffhv7aMEbE3ujP72CgBwYFK4EEACOhgYkDgYYABAC0boVF\r\n" \
"jkFbZl0OlKNLUvEDmiDAfpew3SKZvwtzm+7JnZ8OPW5xKsbubHuQC6vdGioxBBys\r\n" \
"d24UVQ/cMTRAJXcAywBujo5UQgCZF+/ComCy4qznOGpMoF5cdm89to+ycJCiWomj\r\n" \
"+IUGTsaq+P7gDN4xIqERONgcDCyL2irAjn9CGNy8ZQ==\r\n" \
"-----END EC PRIVATE KEY-----";

static const char ecp4_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICHTCCAX8CCQCGSyB7r64S8DAKBggqhkjOPQQDBDBTMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMTcxMjI3MDE0MzM3WhcNMjcxMjI1MDE0\r\n" \
"MzM3WjBTMQswCQYDVQQGEwJ2bjEQMA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwG\r\n" \
"c3V6aG91MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwgZswEAYH\r\n" \
"KoZIzj0CAQYFK4EEACMDgYYABAC0boVFjkFbZl0OlKNLUvEDmiDAfpew3SKZvwtz\r\n" \
"m+7JnZ8OPW5xKsbubHuQC6vdGioxBBysd24UVQ/cMTRAJXcAywBujo5UQgCZF+/C\r\n" \
"omCy4qznOGpMoF5cdm89to+ycJCiWomj+IUGTsaq+P7gDN4xIqERONgcDCyL2irA\r\n" \
"jn9CGNy8ZTAKBggqhkjOPQQDBAOBiwAwgYcCQWZRf+AwD1Pb7SazoBo40MJb+fSV\r\n" \
"uOp/EoBUJYdajpMhSqb8w/YCt0QBqaZ4/VUUK8kIzje7amV+5nAE4GMZCu8jAkIA\r\n" \
"xkw9Q0y3Tq+vFpqZ8phYsMOHuOyH+pC1URoPZ84UFjY4bXxGBkgidssAQX+Ifv7C\r\n" \
"qy9iT3xxY01Hqnsw0vHAlr8=\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char rsa_chain_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDnjCCAoagAwIBAgIJAO5LTLkm2boqMA0GCSqGSIb3DQEBCwUAMGQxCzAJBgNV\r\n" \
"BAYTAmNuMRAwDgYDVQQIDAdqaWFuZ3N1MQ8wDQYDVQQHDAZzdXpob3UxITAfBgNV\r\n" \
"BAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEPMA0GA1UEAwwGUm9vdENBMB4X\r\n" \
"DTE4MDEwMjAzNDk0NloXDTI3MTIzMTAzNDk0NlowZDELMAkGA1UEBhMCY24xEDAO\r\n" \
"BgNVBAgMB2ppYW5nc3UxDzANBgNVBAcMBnN1emhvdTEhMB8GA1UECgwYSW50ZXJu\r\n" \
"ZXQgV2lkZ2l0cyBQdHkgTHRkMQ8wDQYDVQQDDAZSb290Q0EwggEiMA0GCSqGSIb3\r\n" \
"DQEBAQUAA4IBDwAwggEKAoIBAQCtU69XHyMtK52gfpJTKbLgkRKJX7LgPMmU/5EZ\r\n" \
"oIcH5F/Bz9PlAnq5d4jOL5q5hmQJsxm+BNe3PWgC4QaGi7Dq09HBCu7Ytps6RJP2\r\n" \
"Y8Ju6WuD26FQFEZF1b7bTiIzQlbd7knZ+KYzS+tlAjdZA757nj1XZZC1UabwLVOr\r\n" \
"bmpEX9dRWuudpByTnDxO5e1/4JrsWtBnBjSYAZ0u3fXGnw9L14Ja/al3E57OGyfZ\r\n" \
"tWfqNIKpxKsqjm7Ypr8Qw3JCSY2yyFsKLkoCse6yeY0Res9l4yYEYXsLHKHSZULk\r\n" \
"W/Xd5tMm0l9Gl14IvckRvBoioJ2AtE4I3CiunaDMtbQk9lgzAgMBAAGjUzBRMB0G\r\n" \
"A1UdDgQWBBTVY2rhh62z0zezTnM8KfkDK84QpTAfBgNVHSMEGDAWgBTVY2rhh62z\r\n" \
"0zezTnM8KfkDK84QpTAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IB\r\n" \
"AQAt18GsAH8/KdSLZ0OCpM8hrK2zat6lvPxD9K6RwzIHViroF4G/q09r0cRj4kbg\r\n" \
"Zr4AjxRbfovOepyBqdyokR1fgYuTDuIL/GyPhA0f/5KzKwugE4zW6H17rq0jmV8g\r\n" \
"lyAWgwkRd5NUlcBCPmRjYbzeSUfV3hTG8w8DKBzZc1g5c7btTl+0rIWxfMTvofWI\r\n" \
"L/k46K6U1kmxqQrC9YmpU9C+XzSMaeNPdoWN4YCtki7uDwCgg+FWvKW4To4ge6hR\r\n" \
"ZOW+CuqFRqkiTTFCW3fe1d8n1mAYumxgnXh2UDrilWllfdqfsRf4pYx4wxzgCSqU\r\n" \
"nYsMpg5c5XDhinkU/pm8grai\r\n" \
"-----END CERTIFICATE-----\r\n";

static const char rsa_chain_cli_key[]=
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIIEowIBAAKCAQEA1oDhug/JPd2j/8NVAVKIC5q2cqx1CSSpMyqopbvwyS234r/M\r\n" \
"YLg2qQL4n+T90eseslcuIpE/GLkBn/tEpTiyhkTya40bs6hxZyVlxw9d6udv9KWf\r\n" \
"L7gWctzvF7YKvHafKd6eTvnbPvoYULRPGAclObY6iDlQGf7mcaPJt16V/cv4Dsy6\r\n" \
"VdIwl4Z/VLXnvKtFLSGyxm6k1342duSG1poimEWveVvtXxJbx/FnTvK3Euwk8oCc\r\n" \
"yAi0VQXasBvHb3iMahmFgmyIpRuf7Y2eTJbWr697Oh2xJ3ZS/+0KPiT77dY4opsK\r\n" \
"WmaBLPccFvTKNASaOlbFDri7dRVDqMApTMa+JwIDAQABAoIBABgfQkM0R0NHy4uV\r\n" \
"odeWOmpjqaTXtAmlL8nrQC/zJK7bD1TWkNtHnA4BoYgJrmmO8VLI7w1AHEJhe8dL\r\n" \
"8HpMF/d1hXWG1tirVqVs1Tc/iSZDhJmUjoRFRXEBW3nlJOvwwxFoUZE8E+XPJjDd\r\n" \
"Ng/TreIBli9/knx559xbuPGAF4oE0zSomWFHTGRrCrlM3mTZE0kpXXQdZWPzWVx7\r\n" \
"6v7u9UzOypycinn+5L2d7EImbk6qArNvqwMxcb2eUWbugASGgbkQ50sbASoP7/Tj\r\n" \
"ufGtiNVRoOXAa5s0CinatbvDyJ2FKeQfnVhbJ2CU44Fas3BMFv6oa7OZpKpRHySl\r\n" \
"2HSMngECgYEA/GtPmtP3HoJyOhIiHHtiK8PbooGdI3iDJI2v7gTN/Zp84sYdlscK\r\n" \
"QOFFlXS1rowxjs8MldtF1ZFg1GZLDbl802OrnGt51sxiRC5aLU4dmjbU1zXK12st\r\n" \
"fbTyCkZQ14OyS3NlezOBqKCtvfQgzXYq0aHYySd8fC3TKAXHdNt1IqcCgYEA2Yvg\r\n" \
"Hh9AyZCMalh5i2vR87/zXzSr8FawIoUxR8l2aM2RhV/WpmWv68g4S0jzcqPBsc6d\r\n" \
"1TkpyyL04WzmyMAbmerCGL8OnNRPIwerhwl31Dx3xrRRItT1fXxGQEgVkhBSfg5M\r\n" \
"FhC3JLJ8fsdltaNmEdyGPRDwyuqFpwDK0NeueIECgYEA81lJf4zGe2VrVkzREgBp\r\n" \
"nqPOBRdMNTwjwoPOjnuXU90ydTZmpEvzuKT1fqbr2QFTbchy665f3wuu/FY6GVsv\r\n" \
"2mQGuGBtMAYO3oiJa5AZvekKvQghLBkp3D0r83fizEMtCsyPk30OjVGK7syDK1G2\r\n" \
"6i9MXnFw9A8BuV4I8sH4ZZUCgYAvlY/lJtLmA+8ix812OJRI68CRH5VIUb3fHyEj\r\n" \
"Yaa7z9M3yDnMVcvAGCBwHznY5obUnKwbWpCQtUm5ETf9yz9+UEI6ZXgNx8azk9bq\r\n" \
"B6ElVECivwhMRSaZVxiKsPm+5EEW/4uW+/sDVOM0W+scauxFleXq74yr5dZsLBkw\r\n" \
"yyyHgQKBgFijvOksqFNcKXBh58WQhih/11qj4LPQWAeqed7K418R71kTT6QlPrvh\r\n" \
"eTT9K1HTd03bzIwExIjHZRJjI8evS7K/Y7i7BwjBsuRfkRe5A9Xh4lhkFrXOlSWh\r\n" \
"gdJHt05mlhHPvlQEz0/MR1MDnKnUowBChhXvwWe8i5h9Iv0kEFhm\r\n" \
"-----END RSA PRIVATE KEY-----";

static const char rsa_chain_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDrTCCApWgAwIBAgIBBjANBgkqhkiG9w0BAQsFADBkMQswCQYDVQQGEwJjbjEQ\r\n" \
"MA4GA1UECAwHamlhbmdzdTEPMA0GA1UEBwwGc3V6aG91MSEwHwYDVQQKDBhJbnRl\r\n" \
"cm5ldCBXaWRnaXRzIFB0eSBMdGQxDzANBgNVBAMMBlJvb3RDQTAeFw0xODAxMTgw\r\n" \
"NzM2MDVaFw0yODAxMTYwNzM2MDVaMFMxCzAJBgNVBAYTAmNuMRAwDgYDVQQIDAdq\r\n" \
"aWFuZ3N1MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxDzANBgNV\r\n" \
"BAMMBkNsaWVudDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANaA4boP\r\n" \
"yT3do//DVQFSiAuatnKsdQkkqTMqqKW78Mktt+K/zGC4NqkC+J/k/dHrHrJXLiKR\r\n" \
"Pxi5AZ/7RKU4soZE8muNG7OocWclZccPXernb/Slny+4FnLc7xe2Crx2nynenk75\r\n" \
"2z76GFC0TxgHJTm2Oog5UBn+5nGjybdelf3L+A7MulXSMJeGf1S157yrRS0hssZu\r\n" \
"pNd+NnbkhtaaIphFr3lb7V8SW8fxZ07ytxLsJPKAnMgItFUF2rAbx294jGoZhYJs\r\n" \
"iKUbn+2NnkyW1q+vezodsSd2Uv/tCj4k++3WOKKbClpmgSz3HBb0yjQEmjpWxQ64\r\n" \
"u3UVQ6jAKUzGvicCAwEAAaN7MHkwCQYDVR0TBAIwADAsBglghkgBhvhCAQ0EHxYd\r\n" \
"T3BlblNTTCBHZW5lcmF0ZWQgQ2VydGlmaWNhdGUwHQYDVR0OBBYEFKFMHuxqU/F1\r\n" \
"PDp4eM14uMsL8tPbMB8GA1UdIwQYMBaAFNVjauGHrbPTN7NOczwp+QMrzhClMA0G\r\n" \
"CSqGSIb3DQEBCwUAA4IBAQAaGoBgIjZGizGkI7dJQfo5FdJMCJitm6t37KR8FuFf\r\n" \
"59GZ2wMLMbBzvuCotCQ/kL/5JdRzyZzKog0n3UN1DquI9HzxOvhVMXva9XL42hux\r\n" \
"yuvpG17Snyq6KCDVdi/xl1UDBz2bLYxRhVdzyYUXL//rTFj/fozj/JXx8zbm0yyx\r\n" \
"UFEhl2Meysm88h5rHhirhwMRnzY9m6fcFx27D2hNYjbvQGFl927pX186JWfuH3pI\r\n" \
"MFRO7J6pmlVhzJA5oCzRswjd6L68evK1uCRBWE6f/I2y5PTF+ZyKjPCwBDM/cpJ8\r\n" \
"qb5Zwz2/YzNAD8nDfbldeofnafYgQIhSvx/vLBRsE9By\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
static const char ecp_chain_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICmDCCAfugAwIBAgIJAKXBLPcH/FmGMAoGCCqGSM49BAMCMGQxCzAJBgNVBAYT\r\n" \
"AmNuMRAwDgYDVQQIDAdqaWFuZ3N1MQ8wDQYDVQQHDAZzdXpob3UxITAfBgNVBAoM\r\n" \
"GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEPMA0GA1UEAwwGUm9vdENBMB4XDTE4\r\n" \
"MDEwMjAzMjMwMFoXDTI3MTIzMTAzMjMwMFowZDELMAkGA1UEBhMCY24xEDAOBgNV\r\n" \
"BAgMB2ppYW5nc3UxDzANBgNVBAcMBnN1emhvdTEhMB8GA1UECgwYSW50ZXJuZXQg\r\n" \
"V2lkZ2l0cyBQdHkgTHRkMQ8wDQYDVQQDDAZSb290Q0EwgZswEAYHKoZIzj0CAQYF\r\n" \
"K4EEACMDgYYABAF1a0VaYAsuKKKYCf8v8tSJhoSvu+LoYQ+jkUVhVXHW7DvhW+BB\r\n" \
"co6e1EemysIh2pOa5ietQ/dXwoplfPSMvIMJKgDf1fKgnlxZAZqHX44YlKxF18gh\r\n" \
"xo/6FlLa2iiZFfssqq24N7Uy92XH/66yvibSPD4oZudL0mp2KjeFU6Vx98N22qNT\r\n" \
"MFEwHQYDVR0OBBYEFAg9gtXUzqGLz69WHU65gnB7vsjkMB8GA1UdIwQYMBaAFAg9\r\n" \
"gtXUzqGLz69WHU65gnB7vsjkMA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwID\r\n" \
"gYoAMIGGAkE6Pe8RZ72Kv5PfHEI7V0tFDxnzeHn1KiBZeVlhMvcFB8qZZInTQJgB\r\n" \
"IJvxhNgTZGhsNUm+l3uLA6duDDHAFCwW+AJBFpsz2jO3+Vn7sfYNAFp/eYCoK45Y\r\n" \
"8lHVOeJG2huxud7XnAIBH4CPET1uW3u9ZBRClRxtVAyE+6mn+rDLnFHm+FM=\r\n" \
"-----END CERTIFICATE-----";

static const char ecp_chain_cli_key[]=
"-----BEGIN EC PARAMETERS-----\r\n" \
"BgUrgQQAIw==\r\n" \
"-----END EC PARAMETERS-----\r\n" \
"-----BEGIN EC PRIVATE KEY-----\r\n" \
"MIHcAgEBBEIACJ6jRHtHc1AseEc4ORim6zYNHQD4PUg7fD/vZvyrVM7d6kKjNXB1\r\n" \
"0/NScy8dVI92NLzWJGrPWbP0EMKSBRvlsO6gBwYFK4EEACOhgYkDgYYABABGDBMA\r\n" \
"ZgNdrT8DN01aHN7UlsAYjlrUnOPLAewHp5NLPRk6XLkr1f+XjOROPPjvHxJvLZzk\r\n" \
"rpjlpRIBcW0cruWxlQAmkWsKLs2xjv+SeDVVNhKxsnm31p/xeU+6IEx7LYBg5XUe\r\n" \
"b4LPEKZHqGVtaAQhy8QO2wDVdEzjeHkKpSMAWXMkBQ==\r\n" \
"-----END EC PRIVATE KEY-----";

static const char ecp_chain_cli_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIICmTCCAfugAwIBAgICAKgwCgYIKoZIzj0EAwIwVDELMAkGA1UEBhMCY24xEDAO\r\n" \
"BgNVBAgMB2ppYW5nc3UxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0\r\n" \
"ZDEQMA4GA1UEAwwHVGhpcmRDQTAeFw0xODAxMDIwMzM3MjNaFw0yNzEyMzEwMzM3\r\n" \
"MjNaMFMxCzAJBgNVBAYTAmNuMRAwDgYDVQQIDAdqaWFuZ3N1MSEwHwYDVQQKDBhJ\r\n" \
"bnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxDzANBgNVBAMMBkNsaWVudDCBmzAQBgcq\r\n" \
"hkjOPQIBBgUrgQQAIwOBhgAEAEYMEwBmA12tPwM3TVoc3tSWwBiOWtSc48sB7Aen\r\n" \
"k0s9GTpcuSvV/5eM5E48+O8fEm8tnOSumOWlEgFxbRyu5bGVACaRawouzbGO/5J4\r\n" \
"NVU2ErGyebfWn/F5T7ogTHstgGDldR5vgs8QpkeoZW1oBCHLxA7bANV0TON4eQql\r\n" \
"IwBZcyQFo3sweTAJBgNVHRMEAjAAMCwGCWCGSAGG+EIBDQQfFh1PcGVuU1NMIEdl\r\n" \
"bmVyYXRlZCBDZXJ0aWZpY2F0ZTAdBgNVHQ4EFgQU2SWpRA6O1ugkrxG5FtlcKTOA\r\n" \
"nAEwHwYDVR0jBBgwFoAUhQOiEXlTrZW8I99aKtTWecxgquMwCgYIKoZIzj0EAwID\r\n" \
"gYsAMIGHAkE/ZODHXNW2LMC99gFnG4U53LA6KkbBhHotkypDwLNzlAJLc06Sp6h8\r\n" \
"HRcQsFROiW6ImHV+7BWNyQ2AlpsZQ+tFsgJCAevAQRhXHox6A45n66rmZWACEgzh\r\n" \
"9MG5cNZ2T1p3rTxDfjdKxsNl54csA5jv6FAxK2eHaaB9GGEugv1DCLBWMPsK\r\n" \
"-----END CERTIFICATE-----";
#endif

#if defined(TLS_CRT_USED)
/*
you can use this command to get the CA certs:
openssl s_client -showcerts -connect www.baidu.com:443 > baidu.pem
*/
static const char baidu_ca_crt[]=
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIEaTCCA1GgAwIBAgILBAAAAAABRE7wQkcwDQYJKoZIhvcNAQELBQAwVzELMAkG\r\n" \
"A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
"b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw0xNDAyMjAxMDAw\r\n" \
"MDBaFw0yNDAyMjAxMDAwMDBaMGYxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
"YWxTaWduIG52LXNhMTwwOgYDVQQDEzNHbG9iYWxTaWduIE9yZ2FuaXphdGlvbiBW\r\n" \
"YWxpZGF0aW9uIENBIC0gU0hBMjU2IC0gRzIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n" \
"DwAwggEKAoIBAQDHDmw/I5N/zHClnSDDDlM/fsBOwphJykfVI+8DNIV0yKMCLkZc\r\n" \
"C33JiJ1Pi/D4nGyMVTXbv/Kz6vvjVudKRtkTIso21ZvBqOOWQ5PyDLzm+ebomchj\r\n" \
"SHh/VzZpGhkdWtHUfcKc1H/hgBKueuqI6lfYygoKOhJJomIZeg0k9zfrtHOSewUj\r\n" \
"mxK1zusp36QUArkBpdSmnENkiN74fv7j9R7l/tyjqORmMdlMJekYuYlZCa7pnRxt\r\n" \
"Nw9KHjUgKOKv1CGLAcRFrW4rY6uSa2EKTSDtc7p8zv4WtdufgPDWi2zZCHlKT3hl\r\n" \
"2pK8vjX5s8T5J4BO/5ZS5gIg4Qdz6V0rvbLxAgMBAAGjggElMIIBITAOBgNVHQ8B\r\n" \
"Af8EBAMCAQYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUlt5h8b0cFilT\r\n" \
"HMDMfTuDAEDmGnwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0\r\n" \
"dHBzOi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMDMGA1UdHwQsMCow\r\n" \
"KKAmoCSGImh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5uZXQvcm9vdC5jcmwwPQYIKwYB\r\n" \
"BQUHAQEEMTAvMC0GCCsGAQUFBzABhiFodHRwOi8vb2NzcC5nbG9iYWxzaWduLmNv\r\n" \
"bS9yb290cjEwHwYDVR0jBBgwFoAUYHtmGkUNl8qJUC99BM00qP/8/UswDQYJKoZI\r\n" \
"hvcNAQELBQADggEBAEYq7l69rgFgNzERhnF0tkZJyBAW/i9iIxerH4f4gu3K3w4s\r\n" \
"32R1juUYcqeMOovJrKV3UPfvnqTgoI8UV6MqX+x+bRDmuo2wCId2Dkyy2VG7EQLy\r\n" \
"XN0cvfNVlg/UBsD84iOKJHDTu/B5GqdhcIOKrwbFINihY9Bsrk8y1658GEV1BSl3\r\n" \
"30JAZGSGvip2CTFvHST0mdCF/vIhCPnG9vHQWe3WVjwIKANnuvD58ZAWR65n5ryA\r\n" \
"SOlCdjSXVWkkDoPWoC209fN5ikkodBpBocLTJIg1MGCUF7ThBCIxPTsvFwayuJ2G\r\n" \
"K1pp74P1S8SqtCr4fKGxhZSM9AyHDPSsQPhZSZg=\r\n" \
"-----END CERTIFICATE-----";

#endif

#if defined(TLS_CRT_USED)
#define TRUE 1
#define FALSE 0
static const char *ca_cert_str = rsa1_ca_crt;
static const char *cli_key_str = rsa1_cli_key;
static const char *cli_cert_str = rsa1_cli_crt;
/*!
    \brief      initialize ssl client certs
    \param[in]  type: ssl client certs type
                only one parameter can be selected which is shown as below:
      \arg        TLS_CRT_1_RSA_1024_SHA256:
      \arg        TLS_CRT_1_RSA_2048_SHA1:
      \arg        TLS_CRT_1_RSA_3072_SHA256:
      \arg        TLS_CRT_1_ECDSA_PRIME256V1_SHA256:
      \arg        TLS_CRT_1_ECDSA_SECP384R1_SHA384:
      \arg        TLS_CRT_1_ECDSA_BRAINP512R1_SHA512:
      \arg        TLS_CRT_1_ECDSA_SECP521R1_SHA512:
      \arg        TLS_CRT_3_RSA_2048_SHA512:
      \arg        TLS_CRT_3_ECDSA_SECP521R1_SHA512:
      \arg        TLS_CRT_NONE:
    \param[out] none
    \retval     function status: TRUE or FALSE
*/
static int ssl_client_crt_init(unsigned int type)
{
    int ret = TRUE;

    switch(type)
    {
        case TLS_CRT_1_RSA_1024_SHA256:
            ca_cert_str = rsa1_ca_crt;
            cli_key_str = rsa1_cli_key;
            cli_cert_str = rsa1_cli_crt;
            break;
        case TLS_CRT_1_RSA_2048_SHA1:
            ca_cert_str = rsa2_ca_crt;
            cli_key_str = rsa2_cli_key;
            cli_cert_str = rsa2_cli_crt;
            break;
        case TLS_CRT_1_RSA_3072_SHA256:
            ca_cert_str = rsa3_ca_crt;
            cli_key_str = rsa3_cli_key;
            cli_cert_str = rsa3_cli_crt;
            break;
        case TLS_CRT_1_ECDSA_PRIME256V1_SHA256:
            ca_cert_str = ecp1_ca_crt;
            cli_key_str = ecp1_cli_key;
            cli_cert_str = ecp1_cli_crt;
            break;
        case TLS_CRT_1_ECDSA_SECP384R1_SHA384:
            ca_cert_str = ecp2_ca_crt;
            cli_key_str = ecp2_cli_key;
            cli_cert_str = ecp2_cli_crt;
            break;
        case TLS_CRT_1_ECDSA_BRAINP512R1_SHA512:
            ca_cert_str = ecp3_ca_crt;
            cli_key_str = ecp3_cli_key;
            cli_cert_str = ecp3_cli_crt;
            break;
        case TLS_CRT_1_ECDSA_SECP521R1_SHA512:
            ca_cert_str = ecp4_ca_crt;
            cli_key_str = ecp4_cli_key;
            cli_cert_str = ecp4_cli_crt;
            break;
        case TLS_CRT_3_RSA_2048_SHA512:
            ca_cert_str = rsa_chain_ca_crt;
            cli_key_str = rsa_chain_cli_key;
            cli_cert_str = rsa_chain_cli_crt;
            break;
        case TLS_CRT_3_ECDSA_SECP521R1_SHA512:
            ca_cert_str = ecp_chain_ca_crt;
            cli_key_str = ecp_chain_cli_key;
            cli_cert_str = ecp_chain_cli_crt;
            break;
        case BAIDU_CERT:
            ca_cert_str = baidu_ca_crt;
            break;
        case TLS_CRT_NONE:
            ret = FALSE;
            break;
        default:
            ret = FALSE;
            printf("%s(): Type (%d) is unknown.", __func__, type);
            break;
    }
    printf("%s(): type = %d ret = %d\r\n", __func__, type, ret);
    return ret;
}

#endif //#if defined(TLS_CRT_DESC)
