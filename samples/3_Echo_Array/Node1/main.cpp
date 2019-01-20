// std
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <memory>

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

static const char *WORKING_DIRECTORY = "./computer_node1";
static const char *SPACE_ID = "Impro";
static const char *NODE1_ID = "Node1";
static const char *NODE1_CHANNEL_ID = "SendData";
static const char *NODE2_ID = "Node2";
static const char *NODE2_CHANNEL_ID = "ReturnData";


using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace cv;
using namespace impro;

class RecvDataHandler: public ChannelObserver
{
public:
    void dataReceived(Channel &channel)
    {
        cout << "Return: " << channel.currentDataId() << endl;
        unique_ptr<data::ArrayVec3f> ptr(dynamic_cast<data::ArrayVec3f*>(channel.current()));
        for(unsigned long long i = 0; i < ptr->vec_.size(); ++i)
            cout << ptr->vec_.at(i)[0] << ","
                 << ptr->vec_.at(i)[1] << ","
                 << ptr->vec_.at(i)[2] << endl;
    }
};

int main()
{
    srand(time(nullptr));

    // Step1. Prepare and Initialize ImproVisionNet
    AlljoynSpaceBuilder spaceBuilderForAlljoyn;
    data::ArrayVec3f    dataTypeArrayVec3f;
    SpaceBuilder::Prepare(IMPRO_SPACE_TYPE_ALLJOYN, spaceBuilderForAlljoyn);
    impro::DataType::Prepare(IMPRO_DATA_ARRAYVEC3F, dataTypeArrayVec3f);
    Application &app = Application::Initialize();

    // Step2. Join Space, Get Local Node and Register a Channel - "I am node1"
    app.joinSpace(IMPRO_SPACE_TYPE_ALLJOYN,
                  SPACE_ID, NODE1_ID, WORKING_DIRECTORY);
    Space &space = app.getSpace(SPACE_ID);
    LocalNode &node1 = space.getLocalNode();
    LocalChannel &localChannel = node1.registerChannel(IMPRO_DATA_ARRAYVEC3F, NODE1_CHANNEL_ID);

    // Step3. Finding node2 and its channel
    while(!space.hasRemoteNode(NODE2_ID))
    {
        cout << "Finding Remote Node: " << NODE2_ID << " ..." << endl;
        this_thread::sleep_for(1s);
    }
    RemoteNode &node2 = space.getRemoteNode(NODE2_ID);
    while(!node2.hasChannel(NODE2_CHANNEL_ID))
    {
        cout << "Finding Remote Channel: " << NODE2_CHANNEL_ID << " ..." << endl;
        this_thread::sleep_for(1s);
    }
    RemoteChannel &channel = node2.getChannel(NODE2_CHANNEL_ID);
    RecvDataHandler handler;
    channel.subscribe(&handler);

    // Step4. Write Array Data through Local Channel
    while(_getch() != 27)
    {
        milliseconds now = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
        cout << "Send: " << to_string(now.count()) << endl;
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
        localChannel.write(to_string(now.count()), array);
    }

    // Step5. Finialize ImproVisionNet
    Application::Finalize();

    return 0;
}
