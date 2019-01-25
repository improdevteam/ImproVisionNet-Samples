// std
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

// impro
#include <impro/application.hpp>
#include <impro/space.hpp>
#include <impro/node.hpp>
#include <impro/channel.hpp>
#include <impro/channelobserver.hpp>
#include <impro/datatype.hpp>
#include <impro/data/image.hpp>
#include <impro/data/arrayvec3f.hpp>
#include <impro/alljoynspacebuilder.hpp>

// opencv
#include <opencv2/core.hpp>

// getch
#ifdef _WINDOWS_
    #include <conio.h>
#else // for linux
    #include <termios.h>  // for tcxxxattr, ECHO, etc ..
    #include <unistd.h>    // for STDIN_FILENO
    int getch (void)
    {
        int ch;
        struct termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);
        memcpy(&newt, &oldt, sizeof(newt));
        newt.c_lflag &= ~( ECHO | ICANON | ECHOE | ECHOK |
                           ECHONL | ECHOPRT | ECHOKE | ICRNL);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return ch;
    }
#endif

static const char *WORKING_DIRECTORY = "./computer_sender1";
static const char *SPACE_ID = "Impro";
static const char *SENDER_NODE_ID = "Sender1";
static const char *CHANNEL_ID = "ArrayData";

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace cv;
using namespace impro;

int main()
{
    srand(time(nullptr));

    // Step1. Prepare and Initialize ImproVisionNet
    SpaceBuilder::Prepare(IMPRO_SPACE_TYPE_ALLJOYN, AlljoynSpaceBuilder());
    impro::DataType::Prepare(IMPRO_DATA_ARRAYVEC3F, data::ArrayVec3f());
    Application &app = Application::Initialize();

    // Step2. Join Space
    app.joinSpace(IMPRO_SPACE_TYPE_ALLJOYN,
                  SPACE_ID, SENDER_NODE_ID, WORKING_DIRECTORY);
    Space &space = app.getSpace(SPACE_ID);

    // Step3. Get Local Node and Register a Channel
    LocalNode &localNode = space.getLocalNode();
    LocalChannel &localChannel = localNode.registerChannel(IMPRO_DATA_ARRAYVEC3F, CHANNEL_ID);


    // Step4. Write Array Data through Channel
    while(_getch() != 27)
    {
        milliseconds now = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
        cout << localNode.getId() << " send data. | dataId: " << to_string(now.count()) << endl;
        data::ArrayVec3f array;
        for(int i = 0; i < rand() % 10 + 1; ++i)
        {
            Vec3d vec(static_cast<double>(rand())/RAND_MAX,
                      static_cast<double>(rand())/RAND_MAX,
                      static_cast<double>(rand())/RAND_MAX);
            array.vec_.push_back(vec);
			cout << vec[0] << ","
                 << vec[1] << ","
                 << vec[2] << endl;
        }
		cout << endl;
        localChannel.write(to_string(now.count()), array);
    }

    // Step5. Finialize ImproVisionNet
    Application::Finalize();

    return 0;
}
