#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_printf          printf
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
#endif /* MBEDTLS_PLATFORM_C */

#include "mbedtls/error.h"
#include "mbedtls/rsa.h"
#include "mbedtls/x509.h"

#include <string.h>

#define DFL_FILENAME            "keyfile.key"

#define USAGE \
    "\n usage: ec_pub_key_app param=<>...\n"            \
    "\n acceptable parameters:\n"                       \
    "    filename=%%s         default: keyfile.key\n"   \
    "\n"

/*
 * global options
 */
struct options
{
    const char *filename;       /* filename of the key file             */
} opt;

int main( int argc, char *argv[] )
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    char buf[1024];
    int i;
    char *p, *q;

    mbedtls_pk_context pk;

    /*
     * Set to sane values
     */
    mbedtls_pk_init( &pk );
    memset( buf, 0, sizeof(buf) );


    if( argc == 0 )
    {
    usage:
        mbedtls_printf( USAGE );
        goto cleanup;
    }

    opt.filename            = DFL_FILENAME;

    for( i = 1; i < argc; i++ )
    {
        p = argv[i];
        if( ( q = strchr( p, '=' ) ) == NULL )
            goto usage;
        *q++ = '\0';

        if( strcmp( p, "filename" ) == 0 )
            opt.filename = q;
        else
            goto usage;
    }

    fflush( stdout );

    ret = mbedtls_pk_parse_public_keyfile( &pk, opt.filename );

    if( ret != 0 )
    {
        mbedtls_printf( " failed\n  !  mbedtls_pk_parse_public_keyfile returned -0x%04x\n", -ret );
        goto cleanup;
    }

    if( mbedtls_pk_get_type( &pk ) == MBEDTLS_PK_ECKEY )
    {
        char s[ MBEDTLS_MPI_RW_BUFFER_SIZE ];
        size_t n;
        mbedtls_ecp_keypair *ecp = mbedtls_pk_ec( pk );
        mbedtls_printf("04");

        memset( s, 0, sizeof( s ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_string(&ecp->Q.X, 16, s, sizeof( s ) - 2, &n ) );
        mbedtls_printf("%s", s);

        memset( s, 0, sizeof( s ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_write_string(&ecp->Q.Y, 16, s, sizeof( s ) - 2, &n ) );
        mbedtls_printf("%s", s);

        mbedtls_printf("\n");
    }
    else
    {
        mbedtls_printf("Do not know how to print key information for this type\n" );
        goto cleanup;
    }

    exit_code = MBEDTLS_EXIT_SUCCESS;

cleanup:

#if defined(MBEDTLS_ERROR_C)
    if( exit_code != MBEDTLS_EXIT_SUCCESS )
    {
        mbedtls_strerror( ret, buf, sizeof( buf ) );
        mbedtls_printf( "  !  Last error was: %s\n", buf );
    }
#endif

    mbedtls_pk_free( &pk );

    return( exit_code );
}

