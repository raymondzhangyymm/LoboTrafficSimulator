#include <QApplication>
#include <MainWindow.h>

int main(int argc, char** argv)
{
  QApplication application(argc,argv);

  MainWindow vi;

  vi.show();

  return application.exec();
}
