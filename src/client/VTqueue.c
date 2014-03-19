/*
 * TODO - commands:
 *
 * COMMAND_LIST    1
 * COMMAND_INSERT  2
 * COMMAND_REMOVE  3
 * COMMAND_PLAY    4
 * COMMAND_PAUSE   5
 * COMMAND_STOP    6
 * COMMAND_NEXT    7
 * COMMAND_PREV    8
 * COMMAND_MUTE    9
 *
 */

#include "VTqueue.h"

static int debug = 0;

static void VT_command_init(VTCommand *cmd)
{
    if(!cmd) return;
    memset(cmd, 0, sizeof(VTCommand));
    cmd->idx = -1;
}

static int VT_build_command_string(VTCommand *cmd, char *buf, int size)
{
    if(!cmd || !buf || !size) return -1;

    memset(buf, 0, size);
    switch(cmd->cmd) {
        case ADD:
            snprintf(buf, size, "%d %s;%d", 
                    COMMAND_INSERT, cmd->uri, cmd->idx);
            break;
        case REM:
            snprintf(buf, size, "%d %d", COMMAND_REMOVE, cmd->idx);
            break;
        case LIST:
            snprintf(buf, size, "1");
            break;
    }

    return 0;
}

static int VT_send_command(VTCommand *cmd)
{
    int r;
    char buffer[256];
    FILE *fp;
    int fd;
    char *p;
    struct sockaddr_un s;

    r = VT_build_command_string(cmd, buffer, sizeof(buffer));
    if(r < 0)
        return -1;

    s.sun_family = AF_UNIX;
    snprintf(s.sun_path, sizeof(s.sun_path), UNIX_PATH);

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if(connect(fd, (struct sockaddr *) &s, sizeof(s)) < 0) {
        perror("connect");
        exit(1);
    }

    if(!send_cmd(fd, buffer))
        fprintf(stderr, "error: ");

    if(!(fp = fdopen(fd, "r"))) {
        perror("fdopen");
        exit(1);
    }

    while((p = get_cmd_result(fp)) != NULL)
        printf("%s", p);

    //sleep(1);
    shutdown(fd, 2);
    close(fd);
    fclose(fp);

    return 0;
}

int main(int argc, char **argv)
{
    VTCommand cmd;
    int c, optind = 0;
    const char *opts = "a:r:p:ldh";
    const struct option optl[] = {
        { "add",      1, 0, 'a' },
        { "remove",   1, 0, 'r' },
        { "position", 1, 0, 'p' },
        { "list",     0, 0, 'l' },
        { "debug",    0, 0, 'd' },
        { "help",     0, 0, 'h' },
    };

    void show_help() {
        fprintf(stdout, "use: %s OPTIONS\n"
                "NOTES  : There are a _lot_ of commands to come.\n"
                "OPTIONS:\n"
                "\t--add,      -a URI       Add URI to server's play queue\n"
                "\t--remove,   -r URI       Remove URI from server's play queue\n"
                "\t--position, -p IDX       Queue's index to remove or add the URI into\n"
                "\t--list,     -l           list URIs on the server's queue\n"
                "\t--debug,    -d           run de debug mode\n"
                "\t--help,     -h           this help\n", *argv);

        exit(EXIT_SUCCESS);
    }

    VT_command_init(&cmd);
    while((c = getopt_long(argc, argv, opts, optl, &optind)) != -1) {
        switch(c) {
            case 'a':
                cmd.cmd = ADD;
                if(optarg == NULL)
                    show_help();
                snprintf(cmd.uri, sizeof(cmd.uri), "%s", optarg);
                break;
            case 'r':
                cmd.cmd = REM;
                if(optarg == NULL)
                    show_help();
                snprintf(cmd.uri, sizeof(cmd.uri), "%s", optarg);
                break;
            case 'p':
                if(optarg == NULL)
                    show_help();
                cmd.idx = atol(optarg);
                break;
            case 'l':
                cmd.cmd = LIST;
                break;
            case 'd':
                debug = 1;
                break;
            case 'h':
                show_help();
                break;
            default:
                show_help();
        }
    }

    if((cmd.cmd == ADD && (strlen(cmd.uri) < 2 || cmd.idx < 0)) || 
            (cmd.cmd == REM && (strlen(cmd.uri) < 2 || cmd.idx < 0)))
        show_help();

    VT_send_command(&cmd);
    return EXIT_SUCCESS;
}
