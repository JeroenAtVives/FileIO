/* Show the CGI (Common Gateway Interface) environment variables */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include <unistd.h>


#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

#define MAXLEN 80
#define EXTRA 5
/* 4 for field name "data", 1 for "=" */
#define MAXINPUT MAXLEN+EXTRA+2

off_t lseek(int fd, off_t offset, int whence);

void WriteToJSONFile(long data,const char * bestandsNaam);

/* Print a basic HTTP header. */

static void
print_http_header (const char * content_type)
{
    printf ("Content-Type: %s\n\n", content_type);
}

/* Handle errors by printing a plain text message. */

static void cgi_fail (const char * message)
{
    print_http_header ("text/plain");
    printf ("%s\n", message);
    exit (0);
}

/* The external variable "environ" contains the environment
   variables. */

extern char ** environ;

/* This structure will contain our environment variables. The
   nomenclature follows that of 'man getenv'. */

typedef struct
{
    char * name;
    char * value;
}
env_var;

/* Find the first equals sign ("=") in "s". Returns -1 if there is no
   equals sign in the string. */

static int find_equals (const char * s)
{
    int i = 0;
    while (1) {
	if (s[i] == '=') {
	    return i;
	}
	if (s[i] == '\0') {
	    return -1;
	}
	i++;
    }
}

/* Split an environment variable into the part before the equals sign
   and the part after the equals sign. Return the result in the
   env_var structure called "ev". Both are returned in allocated
   memory. */

static void
split_env_var (const char * variable, env_var * ev)
{
    int equals_pos;
    int env_var_len;

    equals_pos = find_equals (variable);
    if (equals_pos == -1) {
	cgi_fail ("could not parse environment");
    }
    env_var_len = strlen (variable);
    ev->name = strndup (variable, equals_pos);
    if (ev->name == 0) {
	cgi_fail ("out of memory");
    }
    ev->value =
	strndup (variable + equals_pos + 1, env_var_len - equals_pos - 1);
    if (ev->value == 0) {
	cgi_fail ("out of memory");
    }
}

/* Catch an environment variable overflow. */

#define _ENV_VAR_GIVE_UP 1000

/* Count the number of environment variables. */

static int
count_env_vars ()
{
    int i;
    for (i = 0; i < _ENV_VAR_GIVE_UP; i++) {
	if (environ[i] == 0) {
	    return i;
	}
    }
    cgi_fail ("too many environment variables");
    return 0;
}

static void get_environment (env_var * env_vars, int n_env_vars)
{
    int i = 0;
    for (i = 0; i < n_env_vars; i++) {
        split_env_var (environ[i], env_vars + i);
    }
}

void unencode(char *src, char *last, char *dest)
{
 for(; src != last; src++, dest++)
    
   if(*src == '+')
     *dest = ' ';
   else if(*src == '%') {
     int code;
     if(sscanf(src+1, "%2x", &code) != 1) code = '?';
     *dest = code;
     src +=2; }     
   else
     *dest = *src;
 *dest = '\n';
 *++dest = '\0';
}

int 
main (int argc, char *argv[])
{
    int i;
    int n_env_vars;
    env_var * env_vars;

    // Alle environment variabelen tonen...
    printf("%s%c%c\n",
    "Content-Type:text/html;charset=iso-8859-1",13,10);
    n_env_vars = count_env_vars ();
    env_vars = calloc (n_env_vars, sizeof (env_var));
    if (! env_vars) {
        cgi_fail ("out of memory");
    }
    get_environment (env_vars, n_env_vars);
    print_http_header ("text/html");
    printf ("<h1>Environment variables</h1>\n");
    printf ("<table>\n");
    for (i = 0; i < n_env_vars; i++) {
	printf ("<tr><td>%d", i);
	printf ("<td>%s<td>%s",
		env_vars[i].name,
		env_vars[i].value);
    }
    printf ("</table>");

    // Variabele die de bestandslocatie bijhoudt.
    char *bestandsNaam ="/var/www/html/myJSON.json";
    
    bool getMode = false;

    if(getMode){
        // Data via get...
        printf ("<h1>Get arguments</h1>\n");
        char *data;
        long dataveldGet;
        data =getenv("QUERY_STRING");  //"dataveldget=2&dataveldGet2=6"; //getenv("QUERY_STRING");
        if(data == NULL)
            printf("<P>Error! Error in passing data from form to script.\n");
        if(sscanf(data,"dataveldget=%ld",&dataveldGet)!=1)
            printf("<P>Error! Invalid data.\n");
        if(sscanf(data,"dataveldget=%ld",&dataveldGet)==1){
            printf("Argument get : %ld\n",dataveldGet);
            // Voeg die gefilterde data nu toe aan een bestand...
            WriteToJSONFile(dataveldGet,bestandsNaam);
        }
    }else{
        // Data via post...
        // De data wordt naar de standaard input stream (stdin) gestuurd...
        char *lenstr;
        char input[MAXINPUT], data2[MAXINPUT];
        long dataveldPost;//char dataveldPost[50];
        long len;
        printf ("<h1>Post arguments</h1>\n");
        printf("%s%c%c\n",
        "Content-Type:text/html;charset=iso-8859-1",13,10);
        lenstr = getenv("CONTENT_LENGTH");
        // sscanf retourneert het aantal variabelen die gevuld zijn.
        if(lenstr == NULL || sscanf(lenstr,"%ld",&len)!=1 || len > MAXLEN)
            printf("<P>Error in invocation - wrong FORM probably.\n");
        else {
        // Read "len +1" karakters van stdin en plaats ze in input.
            fgets(input, len+1, stdin);
            printf("<P>encoded data:%s\n",input);
        }
        // Converteer naar orginineel...
        unencode(input, input+len, data2);
        printf("<P>unencoded data:%s\n",data2);
        if(sscanf(data2,"dataveldpost=%ld",&dataveldPost)==1){
            printf("<P>Argument post : %ld\n",dataveldPost);  
            // Voeg die gefilterde data nu toe aan een bestand...
            WriteToJSONFile(dataveldPost,bestandsNaam);
        }      
    }
      

    free (env_vars);


    exit(EXIT_SUCCESS);

    return 0;
}

void WriteToJSONFile(long data,const char* bestandsNaam){
    // Enkele variabelen...
    int fd,openFlags;
    ssize_t numRead;
    char buf[BUF_SIZE];

    /* Open input file: een json bestand */
    openFlags = O_CREAT | O_RDWR;
    // opmerking : O_APPEND mag er niet bij, anders doet die lseek zijn functie niet.
    fd = open(bestandsNaam, openFlags); 
    if (fd == -1){
        //errExit("opening file %s", bestandsNaam);
        printf("het bestand kon niet worden geopend\n");        
    }



    /* Lezen en tonen van de huidige inhoud. */
    numRead = read(fd, buf, BUF_SIZE);
    //printf("De vorige inhoud van het bestand is %s \n",buf);        

    /* Het laatste ]-teken zoeken want daarvoor moet nieuwe data komen*/
    char *lastSquareBracket = strrchr(buf,93); // 93 is de decimale vorm van ]
    //printf("lastSquareBracket : %s\n",lastSquareBracket);    
    //printf("length of lastSquareBracket tot einde : %d\n",strlen(lastSquareBracket));

    // Hieronder staan er twee manieren om de offset te wijzigen.
    /*Manier 1: De offset van fd wijzigen tot vlak voor ], ofset bepaald tegenover SEEK_END. */
    int offset = lseek(fd,-1,SEEK_END);

    /*Manier 2: De offset van fd wijzigen tot vlak na de laatste }, ofset bepaald tegenover SEEK_SET*/
    //int offset2 = lseek(fd,strlen(buf)-1-strlen(lastSquareBracket)+1,SEEK_SET);

    /* Doet niet wat het moet doen...
    // Timestamp veld voorbereiden...
    time_t t;
    t = time(NULL);
    char buf3[3];
    snprintf(buf3,"%s", ctime(&t));
    //printf(buf3);
    */

    /* Nieuw "object" toevoegen in string vorm aan de json file */
    char buf2[37];
    sprintf(buf2, ",{\"timestamp\" : \"%s\",\"data\": \"%ld\"}]","Sat Nov 23 16:31:15 2019", data); 
    write(fd,buf2,strlen(buf2));    

    


    if (numRead == -1)
        //errExit("read");

    if (close(fd) == -1)
        //errExit("close input");
    exit(EXIT_SUCCESS);
    
}
