// Herb's Destructor

// Fill in the blanks. According to Herb Sutter, "a base class destructor should be either ___ and __ or ___ and ___."


/*
public, virtual, protected, non-virtual

Any operation that will be performed through the base class interface, and that should behave virtually, should be virtual. If deletion, therefore, can be performed polymorphically through the base class interface, then it must behave virtually and must be virtual. The language requires it- if you delete polymorphically without a virtual destructor, you have to face something that is called undefined behaviour. Something that we always want to avoid. But base classes need not always allow polymorphic deletion. In suchcases, make base class destructor protected and non-virtual. This is a guideline, not a rule.
*/