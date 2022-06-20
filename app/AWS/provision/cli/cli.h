#ifndef _CLI_H_
#define _CLI_H_

#define CLI_OUTPUT_SCRATCH_BUF_LEN    512
#define CLI_INPUT_LINE_LEN_MAX        128
#define CLI_PROMPT_STR                "> "
#define CLI_OUTPUT_EOL                "\r\n"

#define CLI_PROMPT_LEN                ( 2 )
#define CLI_OUTPUT_EOL_LEN            ( 2 )

void Task_CLI( void * pvParameters );

#endif /* ifndef _CLI_H_ */
