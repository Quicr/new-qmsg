#include "Team.hh"

Team::Team()
{

}

Team::~Team()
{
    delete [] channels_arr;
}