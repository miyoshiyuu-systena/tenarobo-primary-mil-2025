#include "SearchGateAction.h"
#include "GateCenterCloser.h"
#include "PerceptionReporter.h"
#include "spikeapi.h"

ActionCall searchGateActionFactory() {
    return [](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool is_gate_found = false;
        ICloser* closer = gateCenterCloserGenerator()();
        closer->init();

        do {
            device->twinWheelDrive.leftPivotTurn(90);
            dly_tsk(100 * 1000);
            
            device->twinWheelDrive.stop();
            dly_tsk(50 * 1000);
            
            PerceptionReporter::getInstance().update(200);
            
            is_gate_found = closer->isClosed();
            
            dly_tsk(50 * 1000);
        } while (!is_gate_found);
        
        delete closer;
    };
}