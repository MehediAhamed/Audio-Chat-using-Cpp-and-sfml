#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <cstring>
#include <iostream>



#include <SFML/Audio/Export.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/System/Time.hpp>
#include <cstdlib>


const sf::Uint8 clientAudioData = 1;
const sf::Uint8 clientEndOfStream = 2;




int main()

{

    //audio tcp port
    sf::Packet packet, packetflag;
    sf::IpAddress ip = "127.0.0.1";
    sf::TcpSocket localSocket;

    sf::Socket::Status status = localSocket.connect(ip, 8080);

    while ((status) != sf::Socket::Done)
    {
        std::cout << "Connecting to socket..." << std::endl;
        sf::Socket::Status status = localSocket.connect(ip, 8080);

        if ((status) == sf::Socket::Done)
        {
            std::cout << "Connected to socket!!!" << std::endl;
            break;
        }

    }




    while (1)
    {

        int flag1 = 2, flag2 = 0;

        if (flag1 == 2)

        {
            flag1 = 0;
            flag2 = 0;

            sf::Uint8 id;
            std::vector<sf::Int16> m_samples;
            std::size_t ok;


            if (localSocket.receive(packet) != sf::Socket::Done)
                std::cout << "Audio wasn't recieved" << std::endl;


            std::size_t sampleCount = (packet.getDataSize() - 1) / sizeof(sf::Int16);

            packet >> id;

            if (id == 1 || id == 2)
            {


                // Extract audio samples from the packet, and append it to our samples buffer
                std::size_t sampleCount = (packet.getDataSize() - 1) / sizeof(sf::Int16);


                std::size_t oldSize = m_samples.size();
                m_samples.resize(oldSize + sampleCount);
                std::memcpy(&(m_samples[oldSize]), static_cast<const char*>(packet.getData()) + 1, sampleCount * sizeof(sf::Int16));

                sf::SoundBuffer b1;

                b1.loadFromSamples(m_samples.data(), sampleCount, 1, 45000);

                std::cout << "Recieving Audio" << std::endl;



                sf::Sound sound;
                sound.setBuffer(b1);
                sound.setVolume(100);
                sound.play();
                sf::sleep(sf::microseconds(b1.getSampleCount() * 22));
                sound.stop();


            }
            else
            {
                // Something's wrong...
                std::cout << "Invalid packet received..." << std::endl;

            }


            if (localSocket.receive(packetflag) != sf::Socket::Done)
                std::cout << "Packet end wasn't recieved" << std::endl;

            packetflag >> flag1;

        }

        else
        {
            flag1 = 1;
        }


        if (flag1 == 1)

        {
            //Send audio

            const sf::Int16* samples;
            sf::SoundBuffer buffer;
            sf::Packet packet2;
            sf::SoundBufferRecorder recorder;

            std::cout << "Please speak up" << std::endl;
            if (!recorder.start(44105))
            {
                std::cerr << "Failed to start recorder" << std::endl;
                return 0;
            }


            sf::sleep(sf::milliseconds(6000));

            recorder.stop();

            buffer = recorder.getBuffer();


            samples = buffer.getSamples();
            std::size_t sCount = buffer.getSampleCount();


            packet << clientAudioData;
            packet.append(samples, sCount * sizeof(sf::Int16));

            // Send the audio packet to the client
            if (localSocket.send(packet2) != sf::Socket::Done)
            {

                std::cerr << "Failed to send end-of-stream packet" << std::endl;
            }

            flag2 = 2;
            packetflag << flag2;

            // Send the end of packet to the client
            if (localSocket.send(packetflag) != sf::Socket::Done)
            {

                std::cerr << "Failed to send end-of-stream packet" << std::endl;
            }


        }

        else
        {
            flag1 = 2;

        }
    }

}