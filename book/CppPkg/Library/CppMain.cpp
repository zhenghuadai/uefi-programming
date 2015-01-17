
extern"C"
{
#include <Uefi.h>
}

int cppMain (  IN int Argc,  IN char **Argv  );
void __do_global_dtors_aux();
void __do_global_ctors_aux();

int
main (
  IN int Argc,
  IN char **Argv
  )
{
   
    __do_global_ctors_aux();

    cppMain(Argc, Argv);

    __do_global_dtors_aux();

    return 0;
}
