#include "../intermediate_representation.hpp"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <format>
#include <thread>

#include "../../thread-pool/thread_pool.hpp"

IntermediateRepresentation::IntermediateRepresentation() : funcIR{ exceptions } {}

std::unique_ptr<IRProgram> IntermediateRepresentation::formIR(const ASTProgram* _program){
    std::unique_ptr<IRProgram> _irProgram = std::make_unique<IRProgram>(IRNodeType::PROGRAM);
    
    const size_t total = _program->getFunctionCount();
    _irProgram->resizeFunctions(total);

    std::atomic<size_t> done{};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    std::mutex irMtx;

    for(size_t i = 0; i < total; ++i){
        threadPool.enqueue([&, _irProgram=_irProgram.get(), i, _function=_program->getFunctionAtN(i)]{
            // generating ir of a function
            std::unique_ptr<IRFunction> _irFunction = funcIR.function(_function);

            {
                // setting function on its position
                std::lock_guard<std::mutex> lock(irMtx);
                _irProgram->setFunctionAtN(std::move(_irFunction), i);
            }

            // check if all ir functions are generated
            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_one();
            }
        });
    }

    {
        // wait untill all ir functions are generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&]{ return done == total; });
    }

    checkErrors(_irProgram.get());

    return _irProgram;
}

void IntermediateRepresentation::checkErrors(const IRProgram* _program) const {
    std::string errors{""};
    for(const auto& _function : _program->getFunctions()){
        const std::vector<std::string>& funcErrors = exceptions.at(_function->getFunctionName());
        for(const auto& err : funcErrors){
            errors += std::format("{}\n", err);
        }
    }
    if(!errors.empty()){
        throw std::runtime_error(errors);
    }
}