#ifndef ark_vm
#define ark_vm

#include <vector>
#include <string>
#include <cinttypes>
#include <algorithm>
#include <optional>
#include <memory>

#include <Ark/VM/Value.hpp>
#include <Ark/VM/Frame.hpp>
#include <Ark/Compiler/Instructions.hpp>
#include <Ark/VM/Plugin.hpp>

namespace Ark
{
    namespace VM
    {
        using namespace Ark::Compiler;

        class VM
        {
        public:
            VM(bool debug=false, bool count_fcall=false);

            void feed(const std::string& filename);
            void feed(const bytecode_t& bytecode);
            void run();

            void loadFunction(const std::string& name, Value::ProcType function);

            template <typename T>
            T get(const std::string& name);
        
        private:
            bool m_debug;
            bool m_count_fcall;
            uint64_t m_fcalls;
            bytecode_t m_bytecode;
            // Instruction Pointer and Page Pointer
            int m_ip;
            std::size_t m_pp;
            bool m_running;
            std::string m_filename;

            std::vector<Value::ProcType> m_ffi;

            std::vector<std::string> m_symbols;
            std::vector<Value> m_constants;
            std::vector<std::string> m_plugins;
            std::vector<SharedLibrary> m_shared_lib_objects;
            std::vector<bytecode_t> m_pages;

            std::vector<std::shared_ptr<Frame>> m_frames;
            std::optional<std::shared_ptr<Frame>> m_saved_frame;

            void configure();
            void initFFI();

            inline uint16_t readNumber()
            {
                return (static_cast<uint16_t>(m_pages[m_pp][  m_ip]) << 8) +
                       (static_cast<uint16_t>(m_pages[m_pp][++m_ip])     );
            }

            inline Frame& frontFrame() { return *m_frames.front(); }
            inline Frame& backFrame()  { return *m_frames.back();  }
            inline Frame& frameAt(std::size_t i) { return *m_frames[i]; }
            inline void createNewFrame() { m_frames.push_back(std::make_shared<Frame>()) ; }

            Value pop();
            void push(const Value& value);

            // instructions
            void nop();
            void loadSymbol();
            void loadConst();
            void popJumpIfTrue();
            void store();
            void let();
            void popJumpIfFalse();
            void jump();
            void ret();
            void call();
            void newEnv();
            void builtin();
            void saveEnv();
        };
    }
}

#endif