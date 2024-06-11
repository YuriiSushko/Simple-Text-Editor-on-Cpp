#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <tuple>
#include <stack>
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
                    if (get<0>(position) == 0) {
                        cursor_position = column-1;
                    }
                    else
                    {
                        cursor_position = column;   
                    }
                    break;
                }
                else
                {
                    cout << "There is no such column in line\n";
                    break;
                }

            }
        }
        if (line != get<0>(position)) {
            cout << "There is no such position in text\n";
        }
    }

};


int Cursor::cursor_position = -1;

class MyTextEditor {
private:
    CharPointer* dynamicArray;
    size_t capacity;
    int length;
    stack<CharPointer*> undoStack;
    stack<CharPointer*> redoStack;

public:
    MyTextEditor() : capacity(64), length(0) {
        dynamicArray = (CharPointer*)calloc(capacity, sizeof(CharPointer));
        if (!dynamicArray) {
            perror("Failed to allocate");
            exit(EXIT_FAILURE);
        }
    }

    void clear_stack(stack<CharPointer*>& stackToClear) {
        while (!stackToClear.empty()) {
            free(stackToClear.top());
            stackToClear.pop();
        }
    }

    ~MyTextEditor() {
        free(dynamicArray);
        
        clear_stack(undoStack);
        clear_stack(redoStack);
    }

    void save_comand(stack<CharPointer*>& stack_to_save) {
        CharPointer* comand = (CharPointer*)calloc(capacity, sizeof(CharPointer));
        if (!comand) {
            perror("Failed to allocate");
            exit(EXIT_FAILURE);
        }
        memcpy(comand, dynamicArray, capacity * sizeof(CharPointer));
        stack_to_save.push(comand);
    }

    void restore_comand(stack<CharPointer*>& stack_to_restore) {
        if (!stack_to_restore.empty()) {
            free(dynamicArray);
            dynamicArray = stack_to_restore.top();
            stack_to_restore.pop();
        }
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

    void to_realloc()
    {
        capacity *= 2;
        
        CharPointer* tempArray = (CharPointer*)realloc(dynamicArray, capacity * sizeof(CharPointer));
        if (!tempArray) {
            perror("Failed to reallocate");
            free(dynamicArray);
            exit(EXIT_FAILURE);
        }  
        dynamicArray = tempArray;
    }

    void appendText(bool ifItNewLine, int start_position = -1) {
        save_comand(undoStack);
        save_comand(redoStack);
        int ch;
        static int index = 0;

        if (start_position != -1) {
            int ch;
            cout << "Enter text to insert:\n";
            while (!ifItNewLine && (ch = getchar()) != '\n') {
                dynamicArray[start_position++].ch = ch;
                Cursor cursor(&length);
                cursor.move_right();
            }
            return;
        }

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
                index++;
                dynamicArray[index].p = dynamicArray[index - 1].p + 1;
            }
            else {
                to_realloc();
                dynamicArray[index].ch = ch;
                index++;
                dynamicArray[index].p = dynamicArray[index-1].p + 1;
            }

            if (ifItNewLine) {
                length = index;
                dynamicArray[index].p = 0;
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

    int importFromFile() {
        char loadstring[100];
        char fileNameToImport[64];
        int line = 0;

        cout << "Enter a filename you want to import data from: ";
        cin >> fileNameToImport;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        FILE* fileImport;
        fopen_s(&fileImport, fileNameToImport, "r");

        if (!fileImport) {
            perror("Error opening file");
        }
        else {
            while (fgets(loadstring,100, fileImport) != NULL) {
                int lineLength = static_cast<int>(strlen(loadstring));

                for (int i = 0; i < lineLength; i++) {
                    if (length < static_cast<int>(capacity) - 1) {
                        dynamicArray[length].ch = loadstring[i];
                        length++;
                        dynamicArray[length].p = dynamicArray[length - 1].p + 1;
                    }
                    else {
                        to_realloc();
                        dynamicArray[length].ch = loadstring[i];
                        length++;
                        dynamicArray[length].p = dynamicArray[length - 1].p + 1;
                    }

                    if (loadstring[i] == '\n') {
                        dynamicArray[length].p = 0;
                        line += 1;
                    }

                    dynamicArray[length].ch = '\0';
                }
            }
            fclose(fileImport);
        }
        return line;
    }

    void Insert_with_replacment(int position) {
        save_comand(undoStack);
        save_comand(redoStack);
        int string_length;

        cout << "Enter the length of string:\n";
        cin >> string_length;
        if (length + string_length > static_cast<int>(capacity) - 1) {
            to_realloc();
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        appendText(false, position);
    }

    void insertByLineAndIndex(int position, CharPointer* bufer = NULL) {
        save_comand(undoStack);
        save_comand(redoStack);
        int ch;
        if (bufer == NULL) {
            cout << "Enter the text to insert:\n";

            while ((ch = getchar()) != '\n') {
                if (capacity - length > 1) {
                    for (int j = length; j >= position; j--) {
                        dynamicArray[j + 1].ch = dynamicArray[j].ch;
                        dynamicArray[j + 1].p = dynamicArray[j].p;
                    }

                    dynamicArray[position].ch = ch;
                    dynamicArray[position].p = dynamicArray[position - 1].p + 1;

                }
                else {
                    to_realloc();

                    for (int j = length; j >= position; j--) {
                        dynamicArray[j + 1].ch = dynamicArray[j].ch;
                        dynamicArray[j + 1].p = dynamicArray[j].p;
                    }
                    dynamicArray[position].ch = ch;
                    dynamicArray[position].p = dynamicArray[position - 1].p + 1;

                }

                position++;
                length++;
            }
        }
        else {
            if (capacity - length > 1) {
                for (int j = length; j >= position; j--) {
                    dynamicArray[j + 1].ch = dynamicArray[j].ch;
                    dynamicArray[j + 1].p = dynamicArray[j].p;
                }

                dynamicArray[position].ch = bufer[position].ch;
                dynamicArray[position].p = dynamicArray[position - 1].p + 1;

            }
            else {
                to_realloc();

                for (int j = length; j >= position; j--) {
                    dynamicArray[j + 1].ch = dynamicArray[j].ch;
                    dynamicArray[j + 1].p = dynamicArray[j].p;
                }
                dynamicArray[position].ch = bufer[position].ch;
                dynamicArray[position].p = dynamicArray[position - 1].p + 1;

            }

            position++;
            length++;
        }

        dynamicArray[length].ch = '\0';
        return;
    }

    void delete_chars(int start_position, int num_chars_to_delete = -1) {
        save_comand(undoStack);
        save_comand(redoStack);
        int new_length = 0;

        if (num_chars_to_delete == -1) {
            cout << "Enter number of chars you want to delete\n";
            cin >> num_chars_to_delete;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (length - start_position < num_chars_to_delete) {
                new_length = length - (length - start_position);
            }
            else {
                new_length = length - num_chars_to_delete;
            }
        }
        else {
            new_length = 1;
        }

        if (new_length < start_position) {
            new_length = start_position;
        }

        for (int i = start_position; i < new_length; i++) {
            dynamicArray[i] = dynamicArray[i + num_chars_to_delete];
        }

        length = new_length;

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

    void undo() {
        if (!undoStack.empty()) {
            save_comand(redoStack);
            restore_comand(undoStack);
            cout << "Undo successfully\n";
        }
        else {
            cout << "There is nothing to undo\n";
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            save_comand(undoStack);
            restore_comand(redoStack);
            cout << "Redo successfully\n";
        }
        else {
            cout << "There is nothing to redo.\n";
        }
    }

    CharPointer* copy(int startpoint, int number_of_symbols) {
        int bufer_index = 0;
        CharPointer* bufer = (CharPointer*)calloc(capacity, sizeof(CharPointer));
        for (int i = startpoint; i <= (startpoint + number_of_symbols); i++) {
            bufer[bufer_index] = dynamicArray[i];
            bufer_index++;
        }

        bufer[bufer_index].ch = '\0';
        bufer[bufer_index].p = bufer_index;
        bufer_index++;
        return bufer;
    }


    CharPointer* cut(int startpoint, int number_of_symbols) {
        CharPointer* bufer = (CharPointer*)calloc(capacity, sizeof(CharPointer));
        bufer = copy(startpoint, number_of_symbols);
        delete_chars(startpoint, number_of_symbols);
        return bufer;
    }

    void paste(int startpoint, CharPointer* bufer) {
        insertByLineAndIndex(startpoint, bufer);
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

    void run(bool if_undo = false) {
        int command;
        int line_to_insert = 0;
        int column_to_insert = 0;
        int line = 0;
        int number_of_symbols = 0;
        CharPointer* bufer = (CharPointer*)calloc(capacity, sizeof(CharPointer));

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

            if (command == -1) {
                break;
            }

            switch (command) {
            case 22:
                cursor.move_right();
                break;
            case 33:
                cursor.move_left();
                break;
            case 8:
                showCommands();
                break;
            case 1:
                cout << "\033[H\033[J";
                cout << "Enter the text to append\n";
                appendText(false);

                cursor.set_cursor_position(make_tuple(line, dynamicArray[length].p), dynamicArray);
                cout << "\033[H\033[J";
                break;
            case 2:
                cout << "\033[H\033[J";
                appendText(true);
                line++;
                cursor.set_cursor_position(make_tuple(line, dynamicArray[length].p), dynamicArray);

                cout << "New line added successfully\n";
                break;
            case 3:
                cout << "\033[H\033[J";
                cout << "Printed text: \n";
                break;
            case 4:
                cout << "\033[H\033[J";
                exportToFile();
                break;
            case 5:
                cout << "\033[H\033[J";
                line += importFromFile();
                cursor.set_cursor_position(make_tuple(line, dynamicArray[length].p), dynamicArray);
                break;
            case 6:
                cout << "\033[H\033[J";
                Insert_with_replacment(cursor.get_position());
                break;
            case 7:
                cout << "\033[H\033[J";
                searchText();
                break;
            case 9:
                cout << "\033[H\033[J";

                cout << "Enter line and column to insert\n";
                cin >> line_to_insert >> column_to_insert;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                cursor.set_cursor_position(make_tuple(line_to_insert, column_to_insert), dynamicArray);
                insertByLineAndIndex(cursor.get_position());
            case 10:
                cout << "\033[H\033[J";
                delete_chars(cursor.get_position());
                break;
            case 11:
                cout << "\033[H\033[J";
                undo();
                break;
            case 12:
                cout << "\033[H\033[J";
                redo();
                break;
            case 13:
                cout << "\033[H\033[J";
                cout << "Please, enter the number of symbols to copy\n";
                cin >> number_of_symbols;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                copy(cursor.get_position(), number_of_symbols);
                cout << "Copied successfully!\n";
                break;
            case 14:
                cout << "\033[H\033[J";
                cout << "Please, enter the number of symbols to copy\n";
                cin >> number_of_symbols;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                bufer = cut(cursor.get_position(), number_of_symbols);
                cout << "Cutted successfully!\n";
                break;
            case 15:
                cout << "\033[H\033[J";

                paste(cursor.get_position(), bufer);
                cout << "Pasted successfully!\n";
                break;
            default:
                cout << "Please enter the commands correctly and without ^ symbol\n";
                break;
            }
            print(cursor.get_position());
        }
    }
};

int main() {
    MyTextEditor editor;
    editor.run();
    return 0;
}