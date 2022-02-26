/*
  name: Connor Strom
  Class: CPTS 360
  Date: 2/10/2022

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

char gpath[128]; // hold token strings
char *name[64];
char *name2[64];
char *name4[64], line[128]; // token string pointers
int n, v, u;     // number of token strings
int pd[2];       // piping array

char dpath[128]; // hold dir strings in PATH
char *dir[64];   // dir string pointers
int ndir;        // number of dirs

int check_for_pipe(char l[200])
{

  char line2[100];
  int hj = 0;
  strcpy(line2, l);

  for (int n = 0; n < 200 && l[n] != '\0'; ++n)
  {
    if (l[n] == '|')
    {
      while (line2[n] != '\0')
      {
        name4[hj] = line2[n + 1];
        name2[n] = '\0';
        ++hj, ++n;
      }

      return n;
    }
  }

  n = 0;

  return n;
}

int tokenize_space(char *pathname) // YOU have done this in LAB2
{                                  // YOU better know how to apply it from now on
  char *s;
  strcpy(gpath, pathname); // copy into global gpath[]
  s = strtok(gpath, " ");
  n = 0;
  while (s)
  {
    // printf("%s", name[n]);
    name2[n++] = s; // token string pointers
    s = strtok(0, " ");
  }
  name2[n] = 0; // name[n] = NULL pointer
}

int tokenize_pipe(char *pathname) // YOU have done this in LAB2
{                                 // YOU better know how to apply it from now on
  char *s;
  strcpy(gpath, pathname); // copy into global gpath[]
  // strtok(gpath, "=");
  s = strtok(0, "|");
  n = 0;
  while (s)
  {
    // printf("%s", name[n]);
    name2[n++] = s; // token string pointers
    s = strtok(0, "|");
  }
  name2[n] = 0; // name[n] = NULL pointer
}

int tokenize_colon(char *pathname) // YOU have done this in LAB2
{                                  // YOU better know how to apply it from now on
  char *s;
  strcpy(gpath, pathname); // copy into global gpath[]
  strtok(gpath, "=");
  s = strtok(0, ":");
  v = 0;
  while (s)
  {
    // printf("%s", name[n]);
    name[v++] = s; // token string pointers
    s = strtok(0, ":");
  }
  name[v] = 0; // name[n] = NULL pointer
}

int execute_function(char *cmd, char *env[])
{
      int r = 0;


      for (int x = 0; x < 10; ++x) // check for < > << and then read or write to "filename" using O_RDONLY || O_WRONLY, also pass code num
      {
        //printf("child sh %d running\n", getpid());

        // make a cmd line = dir[0]/cmd for execve()
        //printf("CMD = %s\n", cmd);
        strcpy(line, name[x]);
        // printf("%s\n", line);
        strcat(line, "/");
        strcat(line, cmd);
        printf("line[%d] = %s\n", x, line);

        // printf("HERE: %s\n", name2[x]);
        r = execve(line, name2, env);
      }

      return r;
}

int main(int argc, char *argv[], char *env[])
{
  int i;
  int pd[2], status, pid;
  char *cmd;
  char myEnv[2000];

  // The base code assume only ONE dir[0] -> "./"
  // YOU do the general case of many dirs from PATH !!!!
  dir[0] = "./";
  ndir = 1;

  char *myDirHome = getenv("HOME");

  printf("%s\n", myDirHome);

  int j = 0, k = 0;
  // cycle through env until null
  while (env[j])
  {
    if (strncmp(env[j], "PATH=", 5) == 0)
    {
      // strcpy(myEnv, env[j]); // found path in ENV, copy into string here
      // printf("HELLO\n");
      printf("%s\n", env[j]);
      break;
    }
    j++;
  }

  tokenize_colon(env[j]);

  ndir = n;

  // printf("%d\n", ndir);

  // show dirs
  for (i = 0; i < ndir; i++)
    printf("dir[%d] = %s\n", i, name[i]);

  int r = 0, fd = 0, p = 2, check = 0;

  char *flow[3], *name3[64], *mySign = "";

  while (1)
  {
    printf("sh %d running\n", getpid());
    printf("cssh % : ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;
    if (line[0] == 0)
      continue;

    n = 0;

    check = 0;

    check = check_for_pipe(line);

    if (check > 0)
    {
      tokenize_pipe(line);
      tokenize_space(line);
      // strcpy(name4[1], name2[3]);
      pipe(pd);
    }

    else
    {
      tokenize_space(line);
    }

    // for (i = 0; i < n; i++)
    //{ // show token strings
    // printf("name[%d] = %s\n", i, name2[i]);
    // }

    /* for (int y = 0; name2[y] != ""; ++y)
          {
            if (strcmp(flow[0], name2[y]) == 0 || strcmp(flow[1], name2[y]) == 0 || strcmp(flow[2], name2[y]) == 0)
              strcpy(mySign, name2[y]);
          }*/

    printf("enter a key to continue : ");
    getchar();

    // char *name2[100];

    cmd = name2[0]; // line = name0 name1 name2 ....

    if (strcmp(cmd, "cd") == 0)
    {
      chdir(name2[1]);
      continue;
    }

    if (strcmp(cmd, "exit") == 0)
      exit(0);

    pid = fork();

    if (pid)
    {
      if (check > 0)
      {
        //printf("HERE\n");
        //close(pd[0]); // WRITER MUST close pd[0]
        //close(1);     // close 1
        dup(pd[1]);   // replace 1 with pd[1]
        close(pd[1]);
        //printf("HERE\n");
        r = execute_function(name2[0], env);
      }

      printf("sh %d forked a child sh %d\n", getpid(), pid);
      printf("sh %d wait for child sh %d to terminate\n", getpid(), pid);
      pid = wait(&status);
      printf("ZOMBIE child=%d exitStatus=%x\n", pid, status);
      printf("main sh %d repeat loop\n", getpid());
    }

    else
    {

      flow[0] = "<", flow[1] = ">", flow[2] = ">>";

      // printf("num = %d\n", n);

      for (int k = 0; k < n; ++k)
      {
        printf("name = %s\n", name2[k]);
        if (strcmp(flow[0], name2[k]) == 0)
        {
          // input case
          close(0); // syscall to close file descriptor 0
          fd = open(name2[k + 1], O_RDONLY);
          name2[k] = 0;
        }

        else if (strcmp(flow[1], name2[k]) == 0)
        {
          // output case

          close(1);
          open(name2[k + 1], O_WRONLY | O_CREAT, 0644);
          name2[k] = 0;
        }

        else if (strcmp(flow[2], name2[k]) == 0)
        {
          // error case
          close(1);
          open(name2[k + 1], O_WRONLY | O_APPEND, 0644);
          name2[k] = 0;
        }
      }


      if (check > 0)
      {

        if (fork() == 0)
        {
          //close(pd[0]); // WRITER MUST close pd[0]
          //close(1);     // close 1
          //dup(pd[1]);   // replace 1 with pd[1]
          //close(pd[1]);
          //r = execute_function(name2[0], env);
        //}*/
        //else
        //{
          //close(pd[1]); // READER MUST close pd[1]
          //close(0);
          dup(pd[0]);   // replace 0 with pd[0]
          close(pd[0]); // close pd[0
          printf("RIGHT HERE LOOK: %s\n", name4[0]);
          //r = execute_function(name4[0], env);
        }
      }


      /*for (int x = 0; x < 10; ++x) // check for < > << and then read or write to "filename" using O_RDONLY || O_WRONLY, also pass code num
      {
        printf("child sh %d running\n", getpid());

        // make a cmd line = dir[0]/cmd for execve()
        printf("CMD = %s\n", cmd);
        strcpy(line, name[x]);
        // printf("%s\n", line);
        strcat(line, "/");
        strcat(line, cmd);
        printf("line = %s\n", line);

        // printf("HERE: %s\n", name2[x]);

        r = execve(line, name2, env);
      }*/

    //else
      r = execute_function(cmd, env);

      // else
      //}
    }

    // search based on cases and perform action std out/in operations < > >> <

    // printf("%s\n", line);
    // printf("%s\n", name2[0]);

    if (r == -1)
      printf("execve failed r = %d\n", r);

    // exit(1);
  }
}

/********************* YOU DO ***********************
1. I/O redirections:
Example: line = arg0 arg1 ... > argn-1
  check each arg[i]:
  if arg[i] = ">" {
     arg[i] = 0; // null terminated arg[ ] array
     // do output redirection to arg[i+1] as in Page 131 of BOOK
  }
  Then execve() to change image
2. Pipes:
Single pipe   : cmd1 | cmd2 :  Chapter 3.10.3, 3.11.2
Multiple pipes: Chapter 3.11.2
****************************************************/

/*********** scheduler *************/
/*int scheduler()
{
printf("proc %d in scheduler()\n", running->pid);
if (running->status == READY)
enqueue(&readyQueue, running);
printList("readyQueue", readyQueue);
running = dequeue(&readyQueue);
printf("next running = %d\n", running->pid);
}*/
    
