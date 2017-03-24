#include "cgiu.h"

/*------------------------------------------------------------------------------------*/

int countword(char *line, char stop)
{
    int x,count;
    if (line[0] == 0) return(0);
    count = 1;
    for(x = 0; line[x] != 0; x++) count += (line[x] == stop);
    return(count);
}

char *getword(char *line, char stop)
{
    int x,y;
    char *word;
    for(x = 0; (line[x] != 0) && (line[x] != stop); x++) ;   /* get word length      */
    word = (char *) malloc((x+1)*sizeof(char));              /* allocate word buffer */
    for (y = 0; y < x; y++) word[y] = line[y];               /* copy word in buffer  */
    word[x] = 0;                                             /* terminate string     */
    if (line[x]) x++;                                        /* skip stop character  */
    for (y = 0; line[x+y] != 0; y++) line[y] = line[x+y];    /* shift line left      */
    line[y] = 0;
    return(word);
}

char x2c(char *what)                        /* convert special characters hexa-coded */
{
    int digit;
    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0')) ;
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return((char) digit);
}

void unescape_url(char *url)
{
    int x,y;
    for(x = 0, y = 0; url[y] != 0; ++x, ++y) {
        url[x] = url[y];
        if (url[x] == '%') {
            url[x] = x2c(&(url[y+1]));
            y += 2;
        }
    }
    url[x] = 0;
}

void plustospace(char *str)
{
    int x;
    for(x = 0; str[x] != 0; x++) if (str[x] == '+') str[x] = ' ';
}

/*------------------------------------------------------------------------------------*/

void print_html_head(char *title)
{
    printf("Content-type: text/html%c%c",10,10);
    printf("<HTML>\n");
    printf("<HEAD>\n");
    printf("<TITLE>%s</TITLE>\n",title);
    printf("</HEAD>\n");
    printf("<BODY>\n");
}

void print_debug_query(entry *entries, int m, char *qs, int argc, char *argv[])
{
    int x;
    printf("<H3>&Eacute;l&eacute;ments de votre requ&ecirc;te :</H3>\n");
    if (qs == NULL) {
        printf("Aucune <code>QUERY_STRING</code> n'a &eacute;t&eacute; trouv&eacute;e<p>\n");
    } else {
        printf("<code>QUERY_STRING = \"%s\"</code><br>\n",qs);
        printf("<code>CONTENT_LENGTH = %d</code><p>\n",(int) strlen(qs));
    }
    for (x = 1; x < argc; x++)
        printf("<code>argv[%d] = \"%s\"</code><br>\n",x,argv[x]);
    if (argc > 1) printf("<br>\n");
    if (m)
        printf("Vous avez soumis %d paire%s nom/valeur :<p>\n",m,(m == 1) ? "" : "s");
    else
        printf("Vous n'avez soumis aucune paire nom/valeur.<p>\n");
    printf("<UL>\n");
    for (x = 0; x < m; x++)
        printf("<LI> <code>%s = %s</code>\n",entries[x].name,entries[x].val);
    printf("</UL>\n");
    for (x = 1; x < argc; x++)
        printf("argv[%d]=\"%s\"<br>\n",x,argv[x]);
    if (argc == 1) printf ("Pas d'arguments sur la ligne de commande.");
    printf("<p>\n");
}

void print_html_tail()
{
    printf("</BODY>\n");
    printf("</HTML>\n");
}

void exit_test_ok()
{
    print_html_head("Test");
    printf("OK");
    print_html_tail();
    exit(0);
}

/*------------------------------------------------------------------------------------*/

char *get_query_string(int argc, char *argv[])
{
    int l,cl;
    char *qs,*rqm,*tmp;
    if ((rqm = getenv("REQUEST_METHOD")) == NULL) {
        qs = NULL;
        cl = 0;
        for (l = 1; l < argc; l++) {
            tmp = strdup(argv[l]);
            unescape_url(tmp);
            cl += strlen(tmp)+1;
            qs = (char *) realloc((void *) qs,cl*sizeof(char));
            if (l == 1) *qs = 0; else strcat(qs,"&");
            strcat(qs,tmp);
            free(tmp);
        }
    } else if (!strcmp(rqm,"GET")) {
        qs = getenv("QUERY_STRING");
    } else if (!strcmp(rqm,"POST")) {
        cl = atoi(getenv("CONTENT_LENGTH"));
        qs = (char *) malloc(sizeof(char) * (cl+1));
        for (l = 0; (l < cl) && (!feof(stdin)); l++) qs[l] = (char) fgetc(stdin);
        qs[l] = 0;
    } else {
        print_html_head("Request method error");
        printf("This script should be referenced with a METHOD of POST or GET.\n");
        printf("If you don't understand this, see this ");
        printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/");
        printf("fill-out-forms/overview.html\">forms overview</A>.%c",10);
        print_html_tail();
        exit(1);
    }
    return(qs);
}

entry *get_entries(char *query_string, int *nb_entries)
{
    int x,m;
    char *qs;
    entry *entries;
    if ((query_string == NULL) || (query_string[0] == 0)) {
        *nb_entries = 0;
        return(NULL);
    }
    if (!(qs = strdup(query_string))) return(NULL);
    *nb_entries = m = countword(qs,'&');
    if (!(entries = (entry *) malloc(m*sizeof(entry)))) { 
	free(qs);
	return(NULL);
    }
    for(x = 0; x < m; x++) {
        entries[x].val = getword(qs,'&');
        plustospace(entries[x].val);
        unescape_url(entries[x].val);
        entries[x].name = getword(entries[x].val,'=');
    }
    return(entries);
}

/*------------------------------------------------------------------------------------*/

char *getparam(entry *entries, int m, char *name)
{
    int i;
    for (i = 0; i < m; i++) if (!strcmp(entries[i].name,name)) return(entries[i].val);
    return(NULL);
}

char **getparams(entry *entries, int m, char *name, int *nb_params)
{
    int i,n;
    char **params;
    for (i = n = 0; i < m; i++) n += !strcmp(entries[i].name,name);
    *nb_params = n;
    if (n) {
        params = (char **) malloc(n*sizeof(char *));
        for (i = n = 0; i < m; i++)
            if (!strcmp(entries[i].name,name)) params[n++] = entries[i].val;
        return(params);
    }
    return(NULL);
}

int testparamvalue(entry *entries, int m, char *name, char *val)
{
    int i;
    for (i = 0; i < m; i++)
        if (!strcmp(entries[i].name,name) && !strcmp(entries[i].val,val)) return(1);
    return(0);
}

/*------------------------------------------------------------------------------------*/
