#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <tuple>
using namespace std;

struct CharPointer {
    int p;
    char ch;
};

class Cursor {
private:
    int* size;
    static int cursor_position;
public:
    Cursor(int* size) : size(size) { }

    int move_right() {
        if (cursor_position < *size) {
            cursor_position++;
        }
        return cursor_position;
    }

    int move_left() {
        if (cursor_position > 0) {
            cursor_position--;
        }
        return cursor_position;
    }

    int get_position() {
        return cursor_position;
    }

    void set_cursor_position(tuple<int, int> position, CharPointer* dynamic_array) {
        int line = 0;
        int column = 0;

        for (int i = 0; i < *size; i++) {
            if (dynamic_array[i].ch == '\n') {
                line++;
            }

            column++;

            if (line == get<0>(position)) {
                int sub_column = 0;
                while (sub_column != get<1>(position)) {
                    sub_column++;
                    column++;
                }

                if (sub_column == get<1>(position)) {
                    cursor_position = column-1;
                    break;
                }
                else
                {
                    cout << "There is no such column in line\n";
                    break;
                }

            }
            else
            {
                cout << "There is no such position in text\n";
                break;
            }
        }
    }

};


int Cursor::cursor_position = -1;

class MyTextEditor {
private:
    CharPointer* dynamicArray;
    size_t capacity;
    int length;

public:
    MyTextEditor() : capacity(64), length(0) {
        dynamicArray = (CharPointer*)calloc(capacity, sizeof(CharPointer));
        if (!dynamicArray) {
            perror("Failed to allocate");
            exit(EXIT_FAILURE);
        }
    }

    ~MyTextEditor() {
        free(dynamicArray);
    }

    void print(int cursor_position = -1) const {
        for (int i = 0; i < length; i++) {
            if (cursor_position == i) {
                cout << "|";
            }
            cout << dynamicArray[i].ch;
        }
        if (cursor_position == length) {
            cout << "|";
        }
        cout << std::endl;
    }

    void to_realloc() {
        capacity *= 2;
        CharPointer* tempArray = (CharPointer*)realloc(dynamicArray, capacity * sizeof(CharPointer));
        if (!tempArray) {
            perror("Failed to reallocate");
            free(dynamicArray);
            exit(EXIT_FAILURE);
        }
        dynamicArray = tempArray;
    }

    void appendText(bool ifItNewLine) {
        int ch;
        static int index = 0;

        while (true) {
            if (ifItNewLine) {
                ch = '\n';
            }
            else {
                ch = getchar();
            }

            if (!ifItNewLine && ch == '\n') {
                break;
            }

            if (index < static_cast<int>(capacity) - 1) {
                dynamicArray[index].ch = ch;
                dynamicArray[index].p = index;
                index++;
            }
            else {
                to_realloc();
                dynamicArray[index].ch = ch;
                dynamicArray[index].p = index;
                index++;
            }

            if (ifItNewLine) {
                length = index;
                return;
            }
        }

        dynamicArray[index].ch = '\0';
        length = index;
    }

    void exportToFile() {
        char fileName[64];
        cout << "Enter a filename: ";
        cin >> fileName;

        FILE* file;
        fopen_s(&file, fileName, "w");
        if (file != nullptr) {
            for (int i = 0; i < length; i++) {
                fputc(dynamicArray[i].ch, file);
            }
            fclose(file);
            cout << "Text saved successfully!\n";
        }
        else {
            perror("Error opening file");
        }
    }

    void importFromFile() {
        char loadstring[300];
        char fileNameToImport[64];

        cout << "Enter a filename you want to import data from: ";
        cin >> fileNameToImport;

        FILE* fileImport;
        fopen_s(&fileImport, fileNameToImport, "r");

        if (!fileImport) {
            perror("Error opening file");
        }
        else {
            while (fgets(loadstring, 100, fileImport) != NULL) {
                int lineLength = static_cast<int>(strlen(loadstring));

                for (int i = 0; i < lineLength; i++) {
                    if (length < static_cast<int>(capacity) - 1) {
                        dynamicArray[length].ch = loadstring[i];
                        dynamicArray[length].p = length;
                        length++;
                    }
                    else {
                        to_realloc();
                        dynamicArray[length].ch = loadstring[i];
                        dynamicArray[length].p = length;
                        length++;
                    }
                    dynamicArray[length].ch = '\0';
                }
                cout << loadstring;
            }
            fclose(fileImport);
        }
    }

    void insertByLineAndIndex() {
        int line, column;
        int currentLine = 0;
        int currentColumn = 0;

        cout << "Enter the line and column where to insert:\n";
        cin >> line >> column;

        for (int i = 0; i < length; i++) {
            currentColumn++;

            if (dynamicArray[i].ch == '\n') {
                currentLine++;
                currentColumn = 0;
            }

            if (currentColumn == column && currentLine == line) {
                int ch;
                int insertIndex = i + 1;

                cout << "Enter the text to insert:\n";

                while ((ch = getchar()) != '\n') {
                    if (capacity - length > 1) {
                        for (int j = length; j >= insertIndex; j--) {
                            dynamicArray[j + 1] = dynamicArray[j];
                        }

                        dynamicArray[insertIndex].ch = ch;
                        dynamicArray[insertIndex].p = insertIndex;
                    }
                    else {
                        to_realloc();

                        for (int j = length; j >= insertIndex; j--) {
                            dynamicArray[j + 1] = dynamicArray[j];
                        }

                        dynamicArray[insertIndex].ch = ch;
                        dynamicArray[insertIndex].p = insertIndex;
                    }

                    insertIndex++;
                    length++;
                }

                dynamicArray[length].ch = '\0';
                return;
            }
        }
    }

    void searchText() const {
        std::cout << "Enter the text you want to search\n";
        char* stringToSearch;
        char* tempArray;
        char ch;
        size_t lengthOfSearchString = 64;
        int index = 0;

        stringToSearch = (char*)calloc(lengthOfSearchString, sizeof(char));
        tempArray = (char*)calloc(length, sizeof(char));
        if (!tempArray) {
            perror("Failed to allocate memory for tempArray");
            exit(EXIT_FAILURE);
        }

        while ((ch = getchar()) != '\n') {
            if (index < static_cast<int>(lengthOfSearchString)) {
                stringToSearch[index] = ch;
                index++;
            }
            else {
                lengthOfSearchString *= 2;
                char* temp = (char*)realloc(stringToSearch, capacity * sizeof(char));
                if (!temp) {
                    perror("Failed to reallocate");

                    free(temp);
                    exit(EXIT_FAILURE);
                }
                stringToSearch = temp;
                stringToSearch[index] = ch;
                index++;
            }
        }

        for (int i = 0; i < length; i++) {
            tempArray[i] = dynamicArray[i].ch;
        }

        char* positionPointer = tempArray;
        char* searchedStringPointer;
        char* newLinePointer;
        int linePosition = 0;

        if (stringToSearch == NULL) {
            perror("String to search is empty");
            exit(EXIT_FAILURE);
        }

        while ((searchedStringPointer = strstr(tempArray, stringToSearch)) != NULL) {
            if ((newLinePointer = strstr(tempArray, "\n")) != NULL && (searchedStringPointer == NULL || newLinePointer < searchedStringPointer)) {
                linePosition++;
                tempArray = newLinePointer + 1;
            }

            if (searchedStringPointer != NULL) {
                cout << "The string was found at position: " << linePosition << " " << (searchedStringPointer - positionPointer) << endl;
                tempArray = searchedStringPointer + strlen(stringToSearch);
            }
            else {
                break;
            }
        }

        free(stringToSearch);
        free(tempArray);
    }

    void showCommands() const {
        cout << "^1 - Append text symbols to the end\n"
            << "^2 - Start the new line\n"
            << "^3 - Print the current text\n"
            << "^4 - Load the text to file\n"
            << "^5 - Load the text from file\n"
            << "^6 - Insert line by line and text symbol index\n"
            << "^7 - Search text\n"
            << "^8 - to print the command list\n"
            << "^c - Clear the console\n"
            << "^q - quit from program\n";
    }

    void run() {
        int command;
        cout << "Welcome to my simple text editor\nEnter 8 to print the command list\n";
        
        while (true) {
            Cursor cursor(&length);

            cout << "\nEnter the command\n";
            cin >> command;

            if (cin.fail()) { 
                cout << "Invalid input. Please enter an integer.\n";
                cin.clear(); 
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                continue; 
            } 
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (command == 11) {
                break;
            }

            switch (command) {
            case 13:
                print(cursor.move_right());
                break;
            case 14:
                print(cursor.move_left());
                break;
            case 8:
                showCommands();
                break;
            case 1:
                cout << "\033[H\033[J";
                cout << "Enter the text to append\n";
                appendText(false);
                if (cursor.get_position() == -1) {
                    cursor.set_cursor_position(make_tuple(0, length), dynamicArray);
                }
                cout << "\033[H\033[J";
                print(cursor.get_position());
                break;
            case 2:
                cout << "\033[H\033[J";
                appendText(true);
                print(cursor.get_position());
                cout << "New line added successfully\n";
                break;
            case 3:
                cout << "\033[H\033[J";
                cout << "Printed text: \n";
                print(cursor.get_position());
                break;
            case 4:
                cout << "\033[H\033[J";
                exportToFile();
                break;
            case 5:
                cout << "\033[H\033[J";
                importFromFile();
                break;
            case 6:
                cout << "\033[H\033[J";
                insertByLineAndIndex();
                print(cursor.get_position());
                break;
            case 7:
                searchText();
                break;
            default:
                cout << "Please enter the commands correctly and without ^ symbol\n";
                break;
            }
        }
    }
};

int main() {
    MyTextEditor editor;
    editor.run();
    return 0;
}