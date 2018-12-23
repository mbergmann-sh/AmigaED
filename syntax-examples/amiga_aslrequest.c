/* filereq.c - compile me with vbcc:
** vc +aos68k -v -O3 -size -lamiga -lauto filereq.c - o filereq_vc
**
** Here's a short example showing how to create a file requester with
** asl.library.  If AslRequest() returns TRUE then the rf_File and
** rf_Dir fields of the requester data structure contain the name and
** directory of the file the user selected.  Note that the user can type
** in the a name for the file and directory, which makes it possible for
** a file requester to return a file and directory that do not
** (currently) exist.
*/
#include <exec/types.h>
#include <exec/libraries.h>
#include <libraries/asl.h>
#include <clib/exec_protos.h>
#include <clib/asl_protos.h>
#include <stdio.h>

#ifdef LATTICE
int CXBRK(void)     { return(0); }  /* Disable Lattice CTRL/C handling */
void chkabort(void) { return; }     /* really */
#endif

UBYTE *vers = "$VER: filereq 37.0";

#define MYLEFTEDGE 0
#define MYTOPEDGE  0
#define MYWIDTH    320
#define MYHEIGHT   400

struct Library *AslBase = NULL;

struct TagItem frtags[] =
{
    ASL_Hail,       (ULONG)"The RKM file requester",
    ASL_Height,     MYHEIGHT,
    ASL_Width,      MYWIDTH,
    ASL_LeftEdge,   MYLEFTEDGE,
    ASL_TopEdge,    MYTOPEDGE,
    ASL_OKText,     (ULONG)"O KAY",
    ASL_CancelText, (ULONG)"not OK",
    ASL_File,       (ULONG)"asl.library",
    ASL_Dir,        (ULONG)"libs:",
    TAG_DONE
};

int main(int argc, char **argv)
{
    struct FileRequester *fr;

    if (AslBase = OpenLibrary("asl.library", 37L))
    {
        if (fr = (struct FileRequester *)
            AllocAslRequest(ASL_FileRequest, frtags))
        {
            if (AslRequest(fr, NULL))
            {
                printf("PATH=%s  FILE=%s\n", fr->rf_Dir, fr->rf_File);
                printf("To combine the path and filename, copy the path\n");
                printf("to a buffer, add the filename with Dos AddPart().\n");
            }
            FreeAslRequest(fr);
        }
        else printf("User Cancelled\n");

        CloseLibrary(AslBase);
    }
	
	return(0);
}