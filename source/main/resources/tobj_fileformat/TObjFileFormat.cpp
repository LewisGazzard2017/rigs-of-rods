
#include "TObjFileFormat.h"

using namespace RoR;

void TObjParser::Prepare()
{
    m_in_procedural_road = false;
    m_line_number = 0;
    m_cur_line = nullptr;
    m_def = std::shared_ptr<TObjFile>(new TObjFile());
}

void TObjParser::ProcessLine(const char* line)
{
    if ((line != nullptr) && (strlen(line) != 0) && (line[0] != '/') || (line[0] != ';'))
    {
        m_cur_line = line; // No trimming by design.
    }
    m_line_number++;
}

void TObjParser::ProcessCurrentLine()
{
    
}