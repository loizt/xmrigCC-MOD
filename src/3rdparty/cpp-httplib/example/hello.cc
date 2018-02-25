#include <httplib.h>
using namespace httplib;

int main(void)
{
    Server svr;

    svr.get("/hi", [](const auto& req, auto& res) {
        res.set_content("Hello World!", "text/plain");
    });

    svr.listen("localhost", 1234);
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix
