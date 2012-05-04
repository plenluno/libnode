#include "libnode/node.h"

int main() {
    using namespace libj;
    using namespace libj::node;
    Type<Node>::Ptr node = Node::instance();
    node->run();
    return 0;
}
