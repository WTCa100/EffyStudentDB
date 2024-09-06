#pragma once

class session
{
private:
    /* - data (a data class that will hold info about all DB data + recent changes made) 
       - menu (a class that will be in charge of displaying the UI for the user)
       - input-handler ptr (a class that will handle the user input)
       - Workspace Manager ptr (a class that is in charge of delegating jobs to respected sub managers)*/
public:
    session(/* args */);
    ~session();
};
