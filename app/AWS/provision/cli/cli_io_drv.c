/*
 * FreeRTOS STM32 Reference Integration
 * Copyright (C) 2020-2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "FreeRTOS.h"

#include "cli.h"
#include "cli_prv.h"
#include "logging.h"

#include <string.h>

static char pcOutputBuffer[ 2048 - CLI_OUTPUT_EOL_LEN ];
static char pcInputBuffer[ CLI_INPUT_LINE_LEN_MAX ];

BaseType_t xInitConsole( void )
{
    return 1;
}

static void io_write( const void * const pvOutputBuffer,
                      uint32_t xOutputBufferLen )
{
    strncpy ( pcOutputBuffer, pvOutputBuffer, xOutputBufferLen );
    if( xOutputBufferLen < CLI_OUTPUT_SCRATCH_BUF_LEN )
    {
        pcOutputBuffer[ xOutputBufferLen ] = 0;
    }
    printf( "%s", pcOutputBuffer );
}

static void io_print( const char * const pcString )
{
    printf( "%s", pcString );
}

enum
{
    CNTLC     = 0x03,
    BACKSPACE = 0x08,
    LF        = 0x0A,
    CR        = 0x0D,
    CNTLQ     = 0x11,
    CNTLS     = 0x13,
    ESC       = 0x1B,
    DEL       = 0x7F 
};

static int32_t io_readline( char ** const ppcInputBuffer )
{
    int32_t cnt = 0;
    char * line = pcInputBuffer;
    char c;

    printf( CLI_PROMPT_STR );

    do
    {
        c = ( char )getchar();
        switch( c )
        {
            case CNTLC:                     /* Control C                      */
                printf ( CLI_OUTPUT_EOL CLI_PROMPT_STR );
                break;
            case CNTLQ:                     /* ignore Control S/Q             */
            case CNTLS:
                break;
            case BACKSPACE:
            case DEL:
                if( cnt == 0 )
                {
                    break;
                }
                cnt--;                      /* decrement count                */
                line--;                     /* and line pointer               */
                putchar( BACKSPACE );       /* echo backspace                 */
                putchar( ' ' );
                putchar( BACKSPACE );
                fflush ( stdout );
                break;
            case CR:                        /* CR - done, stop editing line   */
                putchar( c );               /* echo character                 */
                putchar( LF );
                fflush( stdout );
                break;
            default:
                putchar( *line = c );       /* echo and store character       */
                fflush( stdout );
                line++;                     /* increment line pointer         */
                cnt++;                      /* and count                      */
                break;
        }
    } while( ( cnt < ( CLI_INPUT_LINE_LEN_MAX - 1 ) )  && ( c != CR ) );

    *line = 0;                              /* mark end of string             */

    *ppcInputBuffer = pcInputBuffer;

    return cnt;
}

const ConsoleIO_t xConsoleIO =
{
    .read         = NULL,
    .write        = io_write,
    .lock         = NULL,
    .unlock       = NULL,
    .read_timeout = NULL,
    .print        = io_print,
    .readline     = io_readline
};
