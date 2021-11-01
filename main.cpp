#include <iostream>
#include <SFML/Network.hpp>


using namespace std;
sf::Mutex m1;
int main()
{
    sf::TcpSocket* socket = new sf::TcpSocket;
    cout << "server[s] or client[c]" << endl;
    char type;
    cin >> type;
    while((type != 'c' && type != 's') || cin.fail())
    {
        cout << "enter correct" << endl;
        if(cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
        }
        cin >> type;
    }
    cout << "enter name: ";
    string name;
    cin >> name;
    sf::TcpListener listener;
    sf::IpAddress ipAddress;

    if(type == 's')
    {
        listener.listen(2000);
    }
    if(type == 'c')
    {
        cout << "enter ip address: ";
        cin >> ipAddress;

        while(socket->connect(ipAddress, 2000) != sf::Socket::Done)
        {
            cout << "fail connection, enter ip address: ";
            cin >> ipAddress;
        }
    }
    vector<sf::TcpSocket *> clients;
    bool isServerDown = false;
    sf::Thread t1([&](){
        while(true) {
            if(isServerDown) break;
            string message = "";
            getline(cin,message);
            sf::Packet packet;
            packet << name << message;
            if(type == 'c') {
                if (message != "")
                    socket->send(packet);
            }
            else if(type == 's')
            {
                for(int i = 0; i < clients.size(); ++i)
                {
                    clients[i]->send(packet);
                }
            }
            sf::sleep(sf::seconds(0.1f));
        }
    });
    sf::Thread t2([&](){
        while(true)
        {
            string m,n;
            sf::Packet packet;
            if(type == 'c') {
                if(socket->receive(packet) != sf::Socket::Done)
                {
                    if(!isServerDown)
                        cout << "server is down" << endl;
                    isServerDown = true;
                    break;
                }
                packet >> n >> m;
                if (m != "")
                    cout << n << ':' << m << endl;
            }
            sf::TcpSocket s1;
            if(type == 's')
            {
                for(int i = 0; i < clients.size(); ++i)
                {
                    if(clients[i]->receive(packet) == sf::Socket::Disconnected)
                    {
                        cout << clients[i]->getRemotePort() << " disconnected" << endl;
                        delete clients[i];
                        clients.erase(clients.begin() + i);
                    }
                    else
                    {
                        packet >> n >> m;
                        if (m != "")
                            cout << n << ':' << m << endl;
                        for(int j = 0; j < clients.size(); ++j)
                        {
                            if(i != j)
                            {
                                sf::Packet p;
                                p << n << m;
                                clients[j]->send(p);
                            }
                        }
                    }
                }
            }
            sf::sleep(sf::seconds(0.1f));
        }
    });
    sf::Thread t3([&](){
        while(true) {
            if(isServerDown) break;
            if(type == 's') {
                sf::TcpSocket *s = new sf::TcpSocket;
                if (listener.accept(*s) == sf::Socket::Done) {
                    s->setBlocking(false);
                    clients.push_back(s);
                    cout << "connect new client: " << s->getRemotePort() << endl;
                } else {
                    delete s;
                    break;
                }
            }
            sf::sleep(sf::seconds(0.1f));
        }
    });
    t1.launch();
    t2.launch();
    t3.launch();
    if(isServerDown && type == 'c')
        system("pause");
    return 0;
}