/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"
#include "scpKeynodes.hpp"

#include "scpProcessDestroyer.generated.hpp"

namespace scp
{

class ASCPProcessDestroyer : public ScAgent
{
    SC_CLASS(Agent, Event(Keynodes::question_finished, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

public:
    SC_PROPERTY(Keynode("sc_agent_of_scp_process_destruction"), ForceCreate)
    static ScAddr msAgentKeynode;

private:
    void deleteSCPVarsSet(ScAddr & setAddr, ScAddr & processAddr);

};

}

