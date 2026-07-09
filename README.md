# Calc++

Calc++ is a Win32 calculator written in C++ with customizable decimal precision.

The solution contains two projects:

- `calc++` - the main Win32 calculator application.
- `NMath` - a static math library exposing `IMath` and `IRandom` interfaces.

The app links against `NMath.lib` and uses shared public headers from the `shared` folder.
