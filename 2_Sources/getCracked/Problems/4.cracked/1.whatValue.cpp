//  What-value

// Observe the code snippet below. There are 9 lines inside main. 
// Classify each line, starting at the variable declaration of a as either an l-value, r-value, x-value, or pr-value. 


// For example, if there are three lines inside main and you believe the declarations are l-value, x-value, pr-value, your answer should be lxp.
std::string MakeString() {
    return "hello";
}

std::string& Foo(std::string& s) {
    return s;
}

std::string&& Bar(std::string&& s) {
    return std::move(s);
}

int main() {
    std::string a = "world";
    auto&& x1 = a;
    auto&& x2 = MakeString();
    auto&& x3 = std::move(a);
    auto&& x4 = Foo(a);
    auto&& x5 = Bar(MakeString());
    32;
    MakeString();
    int&& y = 42;
}

/*
llllllppl
*/