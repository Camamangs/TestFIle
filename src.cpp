#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <conio.h>

const std::string start_path = "./files";

std::vector<std::string> registry_Info;

bool createFile(const std::string& path){
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)){
        std::cout << "File Exists: " << path << std::endl;
        return false;
    }

    std::ofstream file(path);
    if (file ) {
        file.close();
        return true;
    }
    return false;
}

bool createFolder(const std::string& path){
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)){
        std::cout << "Folder Exists: " << path << std::endl;
        return false;
    }

    std::filesystem::create_directory(path);
    return true;
}

bool readRegistry(std::vector<std::string>& content){
    while(createFile(start_path + "/registry.txt")){
        std::cout << "no Registry Found Created new one" << std::endl;
    }

    std::string path = start_path + "/registry.txt";
    std::ifstream file(path);
    if (file){
        int count = 0;
        std::string lineTemp;
        while (std::getline(file, lineTemp)){

            content.push_back(lineTemp);
        }
    }
    return true;
}

class object{
    public:
        std::string name;
        int matches = 0;
        std::vector<std::string> history;

        std::string getName(){
            return name;
        }

        void matching(object opponent){
            history.push_back(opponent.name);
        }

        void addMatches(){
            matches += 1;
        }

        void createPlayerFiles(){
            bool flag = false;
            while (!flag){
                flag = true;

                if (createFolder(start_path + "/name/" + name) ||
                    createFolder(start_path + "/name/" + name + "/match_history") ||
                    createFolder(start_path + "/name/" + name + "/match_history/match_type") ||
                    createFolder(start_path + "/name/" + name + "/stats") ||
                    createFolder(start_path + "/name/" + name + "/stats/match_type") ||
                    createFile(start_path + "/name/" + name + "/match_history/matches.txt") ||
                    createFile(start_path + "/name/" + name + "/stats/winrate.txt") ||
                    createFile(start_path + "/name/" + name + "/stats/loserate.txt"))

                        flag = false;
            }
        }

        object(std::string nameInfo){
            this -> name = nameInfo;
            if (!checkRegistry() == true || !checkDirectory() == true){

                createPlayerFiles();
            }
            else
                std::cout << "name already exists" << std::endl;
        }

        bool checkRegistry(){
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            std::vector<std::string> list;
            readRegistry(list);

            for (const auto& item : list) {
                std::string temp = item;
                std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

                if (temp == lowerName) {
                    return true;
                }
            }
            std::cout << "notFound" << std::endl;

            std::string sendToFile;
            if (registry_Info.size() > 0)
                for(int i = 0; i < registry_Info.size(); i++){
                    sendToFile += registry_Info[i] + "\n";
                }

            sendToFile += name + "\n";

            std::ofstream file(start_path + "/registry.txt", std::ios::app);
            if(file){
                file << sendToFile;
                file.close();
            }
            return false;
        }

        bool checkDirectory(){
            std::string ownPath = start_path + "/" + name;
            if (std::filesystem::exists(ownPath) && std::filesystem::is_directory(ownPath))
                return true;

            createPlayerFiles();
            return false;
        }
};

void matchmaking(object player1, object player2){
    player1.matching(player2);
    player1.addMatches();

    player2.matching(player1);
    player2.addMatches();
}

void createDirectories(){
    createFolder(start_path);
    createFolder(start_path + "/name");
    createFile(start_path + "/registry.txt");
}

void checkPeopleInRegistry(std::vector<object>& objects) {
    readRegistry(registry_Info);
    for (const auto& info : registry_Info) {
        objects.emplace_back(info);
    }
}

void checkPeopleInDirectories(){
    size_t start = 13;

    std::vector<std::string> placeholder;
    std::vector<std::string> gotNames;

    std::string path = "./files/name";
    for (const auto& entry : std::filesystem::directory_iterator(path)){

        placeholder.push_back(entry.path().string());
    }

    for (const auto& names: placeholder){
        size_t end = names.length();
        gotNames.push_back(names.substr(start, end));
    }


    placeholder.clear();

    std::string registry_path = "./files/registry.txt";
    std::ifstream file(registry_path);
    if (file){
        std::string lineTemp;
        while (std::getline(file, lineTemp)){

            placeholder.push_back(lineTemp);
        }
    }


    std::vector<std::string> namesNotListed;

    if (placeholder.size() > 0){

        for(const auto& item: gotNames){
            bool found = false;
            for(const auto& fileName: placeholder){
                if (item == fileName){
                    found = true;
                    break;
                }
            }

            if(!found){

                namesNotListed.push_back(item);
            }
        }
    }
    else {
        for(const auto& filename: gotNames){
            namesNotListed.push_back(filename);
        }
    }

    
    std::string sendToFile;
    for (const auto& name : placeholder) {
        sendToFile += name + "\n";
    }
    for (const auto& name : namesNotListed) {
        sendToFile += name + "\n";
    }

    std::ofstream file_o(registry_path);
    if(file_o){
        file_o << sendToFile;
        file_o.close();
    }
}

int selectionUI(int characterCatch, int selectionID, std::vector<std::string> List){
    if (characterCatch == 224){

        int catcher = _getch();
        if (catcher == 72 && selectionID-1 > -1){
            return 1;
        }
        else if (catcher == 80 && selectionID+1 < List.size()){
            return 0;
        }
    }

    if (characterCatch == 13){
        return -1;
    }

    return 2;
}
// to function properly needs to be inside in while loop
        // to use needs to be "while(!ListUI(nameList, select));" and partnered with an int to be able to get output
bool ListUI(std::vector<std::string> List, int &currentSelection){
    int ch;
    int keyPressed;

    for (int i = 0; i < List.size(); i++){
        if (i == currentSelection)
            std::cout << "\x1B[107m" << List[i] << "\033[0m" << std::endl;
        else
            std::cout << List[i] << std::endl;
    }
    
    ch = _getch();
    keyPressed = selectionUI(ch, currentSelection, List);
    if (keyPressed == 0)
        currentSelection++;
    if (keyPressed == 1)
        currentSelection--;

    system("cls");
    std::cout << currentSelection << std::endl;

    if (keyPressed == -1)
        return true;

    return false;
}

/*
    Ui Menu

        Add person
        Delete Person
        update Match
            1v1 Match
            2v2 match
                Start Match
                    Add timer for match
                End Match
        Person Information

        Edit Person Info
            ==Authorization needed==
                Put password system

        Exit
*/

void ui(std::vector<object> object){
    int selection = 0;
    std::vector<std::string> nameList;

    for (int i = 0; i < object.size(); i++)
        nameList.push_back(object[i].getName());
    
    //for (const auto& names: object)


    do{
        std::cout << "People Menu" << std::endl;
    }while(!ListUI(nameList, selection));
}

int main(){
    std::vector<object> people;
    createDirectories();
    checkPeopleInDirectories();
    checkPeopleInRegistry(people);

    ui(people); // RE-DO=========

    system("pause");
}