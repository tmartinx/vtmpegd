#ifndef _CONFIG_H
#define _CONFIG_H 1

/* path pro arquivo que o server
   cria (unix domain socket) e
   o client se conecta pra mexer
   no queue */
#define UNIX_PATH "/tmp/VTmpegd"

/* tamanho m�ximo dos comandos
   entre o client e o server */
#define MAX_COMMAND_LEN 20

/* tamannho maximo da msg de result
   do servidor */
#define MAX_RESULT_LINE_LEN 2048

/* defini��es do widget onde dever� passar o mpeg */
#define VIDEO_WIDTH	640
#define VIDEO_HEIGHT	480
#define VIDEO_DEPTH	16

/*
  especifica��o do protocolo

  o client envia:
  1. para listar o queue: list;
  2. para inserir no queue: insert;filename;[pos]
  3. para remover do queue: remove;pos

  o server responde:
  1. pro comando 'list':
  
     COMMAND_OK || COMMAND_ERROR
     pos - filename
     COMMAND_DELIM
     
     Ex:
     COMMAND_OK
     1 - /tmp/video0.mpeg
     2 - /home/video/test.mpeg
     COMMAND_DELIM

  2. pro comando 'insert' e 'remove':

     COMMAND_OK || COMMAND_ERROR
     COMMAND_DELIM

*/
#define COMMAND_OK	'S'
#define COMMAND_ERROR	'E'
#define COMMAND_DELIM	';'

#define COMMAND_LIST    1
#define COMMAND_INSERT  2
#define COMMAND_REMOVE  3
#define COMMAND_PLAY    4
#define COMMAND_PAUSE   5
#define COMMAND_STOP    6
#define COMMAND_NEXT    7
#define COMMAND_PREV    8
#define COMMAND_MUTE    9

#endif /* config.h */
