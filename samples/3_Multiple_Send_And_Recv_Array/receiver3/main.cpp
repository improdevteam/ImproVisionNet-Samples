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
#ifdef _WINDOWS_ // for windows
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

static const char *WORKING_DIRECTORY = "./computer_receiver3";
static const char *SPACE_ID = "Impro";
static const char *SENDER_NODE_ID = "Sender2";
static const char *CHANNEL_ID = "ArrayData";
static const char *RECEIVER_NODE_ID = "Receiver3";

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace cv;
using namespace impro;

class RecvDataHandler: public ChannelObserver
{
public:
    RecvDataHandler() : ChannelObserver() {}

    void dataReceived(Channel &remoteChannel)
    {
        string dataId = remoteChannel.currentDataId();
        shared_ptr<data::ArrayVec3f> ptr = static_pointer_cast<data::ArrayVec3f>(remoteChannel.current());
        cout << "Receive from " << remoteChannel.getNode().getId() << " | dataId: " << dataId << endl;
        cout << "Receive from " << remoteChannel.getNode().getId() << " | dataId: " << dataId << endl;
        for(unsigned long long i = 0; i < ptr->vec_.size(); ++i)
            cout << ptr->vec_.at(i)[0] << ","
                 << ptr->vec_.at(i)[1] << ","
                 << ptr->vec_.at(i)[2] << endl;
        cout << endl;
    }
};

int main()
{
    // Step1. Prepare and Initialize ImproVisionNet
    SpaceBuilder::Prepare(IMPRO_SPACE_TYPE_ALLJOYN, AlljoynSpaceBuilder());
    impro::DataType::Prepare(IMPRO_DATA_ARRAYVEC3F, data::ArrayVec3f());
    Application &app = Application::Initialize();

    // Step2. Join Space
    cout << "test0" << endl;

    app.joinSpace(IMPRO_SPACE_TYPE_ALLJOYN,
                  SPACE_ID, RECEIVER_NODE_ID, WORKING_DIRECTORY);
    Space &space = app.getSpace(SPACE_ID);

    cout << "test1" << endl;

    // Step3. Finding Remote Node From Space
    while(!space.hasRemoteNode(SENDER_NODE_ID))
    {
        cout << "Finding Remote Node: " << SENDER_NODE_ID << " ..." << endl;
        this_thread::sleep_for(1s);
    }

    cout << "test2" << endl;

    // Step3. Finding Remote Channel From Remote Node
    RemoteNode &sender = space.getRemoteNode(SENDER_NODE_ID);
    while(!sender.hasChannel(CHANNEL_ID))
    {
        cout << "Finding Remote Channel: " << CHANNEL_ID << " ..." << endl;
        this_thread::sleep_for(1s);
    }

    cout << "test3" << endl;

    // Step4. Subscribe Remote Channel
    RemoteChannel &channel = sender.getChannel(CHANNEL_ID);
    RecvDataHandler handler;
    channel.subscribe(&handler);

    cout << "test4" << endl;

    // Step5. Main Loop
    while(_getch() != 27)
    {
        this_thread::sleep_for(1s);
    }

    // Step6. Finialize ImproVisionNet
    Application::Finalize();

    return 0;
}
