#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class MyTextEditor {
private:
    struct CharPointer {
        int p;
        char ch;
    };

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

    void print() const {
        for (int i = 0; i < length; i++) {
            std::cout << dynamicArray[i].ch;
        }
        std::cout << std::endl;
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
        std::cout << "Enter a filename: ";
        std::cin >> fileName;

        FILE* file;
        fopen_s(&file, fileName, "w");
        if (file != nullptr) {
            for (int i = 0; i < length; i++) {
                fputc(dynamicArray[i].ch, file);
            }
            fclose(file);
            std::cout << "Text saved successfully!\n";
        }
        else {
            perror("Error opening file");
        }
    }

    void importFromFile() {
        char loadstring[300];
        char fileNameToImport[64];

        std::cout << "Enter a filename you want to import data from: ";
        std::cin >> fileNameToImport;

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
                std::cout << loadstring;
            }
            fclose(fileImport);
        }
    }

    void insertByLineAndIndex() {
        int line, column;
        int currentLine = 0;
        int currentColumn = 0;

        std::cout << "Enter the line and column where to insert:\n";
        std::cin >> line >> column;

        for (int i = 0; i < length; i++) {
            currentColumn++;

            if (dynamicArray[i].ch == '\n') {
                currentLine++;
                currentColumn = 0;
            }

            if (currentColumn == column && currentLine == line) {
                int ch;
                int insertIndex = i + 1;

                std::cout << "Enter the text to insert:\n";

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
                std::cout << "The string was found at position: " << linePosition << " " << (searchedStringPointer - positionPointer) << std::endl;
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
        std::cout << "^1 - Append text symbols to the end\n"
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

        std::cout << "Welcome to my simple text editor\nEnter 8 to print the command list\n";

        while (true) {
            std::cout << "\nEnter the command\n";
            std::cin >> command;

            if (command == 'q') {
                break;
            }

            switch (command) {
            case 8:
                showCommands();
                break;
            case 1:
                std::cout << "\033[H\033[J";
                std::cout << "Enter the text to append\n";
                appendText(false);
                break;
            case 2:
                std::cout << "\033[H\033[J";
                appendText(true);
                std::cout << "New line added successfully\n";
                break;
            case 3:
                std::cout << "\033[H\033[J";
                std::cout << "Printed text: \n";
                print();
                break;
            case 4:
                std::cout << "\033[H\033[J";
                exportToFile();
                break;
            case 5:
                std::cout << "\033[H\033[J";
                importFromFile();
                break;
            case 6:
                std::cout << "\033[H\033[J";
                insertByLineAndIndex();
                break;
            case 7:
                searchText();
                break;
            default:
                std::cout << "Please enter the commands correctly and without ^ symbol\n";
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
