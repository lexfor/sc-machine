/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_addr.hpp"
#include "scpKeynodes.hpp"
#include "scpUtils.hpp"

#include "scpOperator.hpp"

#include <algorithm>
#include <iostream>

namespace scp {

SCPOperator::SCPOperator(const std::unique_ptr<ScMemoryContext>& ctx_, ScAddr addr_): addr(addr_), ms_context(ctx_)
{
}

std::string SCPOperator::GetTypeName()
{
    return "unknown operator";
}

ScAddr SCPOperator::GetAddr()
{
    return addr;
}

sc_result SCPOperator::ResetValues()
{
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        if ((*i)->IsAssign())
        {
            if ((*i)->IsSCPConst())
            {
#ifdef SCP_DEBUG
                Utils::logSCPError(ms_context, "SCP CONST must have FIXED modifier", addr);
#endif
                FinishExecutionWithError();
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
            (*i)->ResetValue();
        }
    }
    return SC_RESULT_OK;
}

sc_result SCPOperator::CheckNullValues()
{
    int k=0;
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        k++;
        if (!(*i))
        {
#ifdef SCP_DEBUG
           // Utils::logSCPError(ms_context, "One or more operands missed", addr);
           // std::cout<<"NUMBER: "<<k<<std::endl;
            Utils::printInfo(ms_context, addr);
#endif
            FinishExecutionWithError();
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
    }
    return SC_RESULT_OK;
}

sc_result SCPOperator::Parse()
{
    std::fill(operands.begin(), operands.end(), nullptr);
    return SC_RESULT_OK;
}

sc_result SCPOperator::Execute()
{
    return SC_RESULT_OK;
}

void SCPOperator::ClearExecutionState()
{
    std::vector<ScAddr> arcs;
    ScIterator3Ptr iter=ms_context->Iterator3(ScType::NodeConst, ScType::EdgeAccessConstPosPerm, addr);
    while (iter->Next())
    {
        if (iter->Get(0)==Keynodes::active_action)
            arcs.push_back(iter->Get(1));
        if (iter->Get(0)==Keynodes::question_finished)
            arcs.push_back(iter->Get(1));
        if (iter->Get(0)==Keynodes::question_finished_successfully)
            arcs.push_back(iter->Get(1));
        if (iter->Get(0)==Keynodes::question_finished_unsuccessfully)
            arcs.push_back(iter->Get(1));
        if (iter->Get(0)==Keynodes::question_finished_with_error)
            arcs.push_back(iter->Get(1));
    }
    for (std::vector<ScAddr>::iterator i = arcs.begin(); i != arcs.end(); ++i)
        ms_context->EraseElement(*i);
}

void SCPOperator::FinishExecution()
{
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished, addr);
}

void SCPOperator::FinishExecutionSuccessfully()
{
    ClearExecutionState();
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished_successfully, addr);
    FinishExecution();
}

void SCPOperator::FinishExecutionUnsuccessfully()
{
    ClearExecutionState();
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished_unsuccessfully, addr);
    FinishExecution();
}

void SCPOperator::FinishExecutionWithError()
{
    ClearExecutionState();
    ms_context->CreateArc(ScType::EdgeAccessConstPosPerm, Keynodes::question_finished_with_error, addr);
    FinishExecution();
}

SCPOperator::~SCPOperator()
{
    for (std::vector<SCPOperand*>::iterator i = operands.begin(); i != operands.end(); ++i)
    {
        delete *i;
    }
}

}

