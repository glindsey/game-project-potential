// *** ADDED BY HEADER FIXUP ***
#include <exception>
#include <iostream>
// *** END ***
/// Main file for the application.
/// @author Gregory Lindsey

#ifdef __cplusplus
#include <cstdlib>
#include <clocale>
#else
#include <stdlib.h>
#include <locale.h>
#endif

#include "Application.h"

/// Main function!
/// Not much explanation required here.  It creates the Application instance,
/// and then calls doProcessLoop to run the processing loop until the program
/// terminates.
int main(int argc, char** argv)
{
  // Set locale information for Unicode text.
  char *locale = setlocale(LC_ALL, "");

  // Start the application instance.
  try
  {
    App& theApp = App::instance();
    theApp.execute();
  }
  catch (std::exception& e)
  {
    std::cout << "Exception thrown: " << e.what() << std::endl;
  }
  catch (boost::exception& e)
  {
    std::cout << "Boost exception thrown: %s" <<
                 boost::diagnostic_information(e) << std::endl;
  }

  return 0;
}
