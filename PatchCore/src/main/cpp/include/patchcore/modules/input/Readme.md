

UserKeyboard - interface for setting gate and cv value
void setGate(float value)
void setCv(float value)
void setVelocity(float value)

AbstractKeyboard - class implements IModule with outputs
    ModuleOutput gate = ModuleOutput(KEYBOARD_OUTPUT_GATE);
    ModuleOutput cv = ModuleOutput(KEYBOARD_OUTPUT_CV);
    ModuleOutput velocity = ModuleOutput(KEYBOARD_OUTPUT_VELOCITY);

CompositeKeyboard - has keyboard and sequencer and implements AbstractKeyboard interface


