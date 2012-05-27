#include "libj/console.h"
#include "libnode/node.h"
#include "libnode/timer.h"


namespace libj {
namespace node {

Int count = 100;

class OnInterval : LIBJ_JS_FUNCTION(OnInterval)
 private:
    Value id_;
    
 public:
    void setId(Value id) {
        id_ = id;
    }
    
    Value operator()(JsArray::CPtr args) {
        if (count) {
            console::log(String::valueOf(count--));
        } else {
            clearInterval(id_);
        }
        return 0;
    }
};

}  // namespace node
}  // namespace libj

int main() {
    using namespace libj;
    using namespace node;
    
    OnInterval::Ptr onInterval(new OnInterval);
    JsArray::Ptr args = JsArray::create();
    Value id = setInterval(onInterval, 100, args);
    onInterval->setId(id);
    
    node::run();
    return 0;
}
