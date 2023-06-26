#include <iostream>
#include <cstring>

namespace nmea {

struct Sentence {
    static constexpr char start = '$';
    static constexpr char field_delimiter = ',';
    static constexpr char checksum_delimiter = '*';
    static constexpr const char *terminator = "\r\n";

    static constexpr uint8_t command_size = 5;
    static constexpr uint8_t max_field_num = 20;
    static constexpr uint8_t max_field_size = 16;
    static constexpr uint8_t checksum_size = 2;
    static constexpr uint8_t terminator_size = 2;

    char command[command_size+1];
    char fields[max_field_num][max_field_size+1];
    uint8_t fields_num;
};

class Parser {

    enum class State {
        START,
        COMMAND,
        FIELDS,
        CHECKSUM,
        TERMINATOR
    };

    State state;
    Sentence buffer;
    char checksum[Sentence::checksum_size+1];
    char terminator[Sentence::terminator_size+1];
    uint8_t field_counter;
    uint8_t char_counter;

public:
    Parser();

    bool consume(const char byte);
    Sentence getSentence() const;
};

}

nmea::Parser::Parser() : state{State::START} {

}

bool nmea::Parser::consume(const char byte) {
    if(state==State::START && byte==Sentence::start) {
        state = State::COMMAND;
        char_counter = 0;
        memset(buffer.command, 0, Sentence::command_size+1);
        memset(buffer.fields, 0, Sentence::max_field_num*(Sentence::max_field_size+1));
        memset(checksum, 0, Sentence::checksum_size+1);
        memset(terminator, 0, Sentence::terminator_size+1);
        return false;
    }

    if(state==State::COMMAND && byte!=Sentence::field_delimiter) {
        buffer.command[char_counter] = byte;
        char_counter++;

        if(char_counter==Sentence::command_size) {
            state = State::FIELDS;
            field_counter = 0;
            char_counter = 0;
        }

        return false;
    }

    if(state==State::FIELDS) {
        if(byte==Sentence::field_delimiter) {
            field_counter++;
            field_counter %=Sentence::max_field_num;
            char_counter = 0;
        } else if(byte==Sentence::checksum_delimiter) {
            state = State::CHECKSUM;
            buffer.fields_num = field_counter;
            field_counter = 0;
            char_counter = 0;
        } else {
            buffer.fields[field_counter-1][char_counter] = byte;
            char_counter++;
            char_counter %=Sentence::max_field_size;
        }

        return false;
    }

    if(state==State::CHECKSUM) {
        checksum[char_counter] = byte;
        char_counter++;

        if(char_counter==Sentence::checksum_size) {

            // check if valid

            state = State::TERMINATOR;
            char_counter = 0;
        }

        return false;
    }

    if(state==State::TERMINATOR) {
        terminator[char_counter] = byte;
        char_counter++;

        if(char_counter==Sentence::terminator_size) {

            state = State::START;

            return (strncmp(terminator, Sentence::terminator, Sentence::terminator_size)==0);
        }
    }

    return false;
}

nmea::Sentence nmea::Parser::getSentence() const {
    return buffer;
}

int main() {

    const char *input = "$GPRMC,235420.00,A,5106.19303,N,01705.07333,E,0.471,,240623,,,A*7C\r\n";
    //const char *input = "$GPGSV,3,1,11,12,31,118,39,14,30,289,44,15,20,059,41,18,68,007,43*75\r\n";
    //const char *input = "$GPGGA,060556.00,2236.91418,N,11403.24669,E,2,08,1.02,115.1,M,-2.4,M,,0000*43\r\n";
    //const char *input = "$GPRMC,060556.00,A,2236.91418,N,11403.24669,E,0.13,309.62,130214,,,D*7F\r\n";

    nmea::Parser parser;

    for(int i=0; i<strlen(input); i++) {

        if(!parser.consume(input[i]))
            continue;

        const nmea::Sentence sentence = parser.getSentence();

        std::cout << sentence.command << std::endl;
        for(int j=0; j<sentence.fields_num; j++)
            std::cout << j << "\t" << sentence.fields[j] << std::endl;
    }

}