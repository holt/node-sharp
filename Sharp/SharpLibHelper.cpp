#include "SharpLibHelper.h"
#using <mscorlib.dll>
#using <SharpLib.dll>
#include <gcroot.h>

using namespace System::Runtime::InteropServices;
using namespace System::Reflection;
using namespace SharpLib;

// we need to look for the assembly in the current directory
// node will search for it next to the node.exe binary
System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^obj, System::ResolveEventArgs ^args)
{
    System::String ^path = System::Environment::CurrentDirectory;
    array<System::String^>^ assemblies =
        System::IO::Directory::GetFiles(path, "*.dll");
    for (long ii = 0; ii < assemblies->Length; ii++) {
        AssemblyName ^name = AssemblyName::GetAssemblyName(assemblies[ii]);
        if (AssemblyName::ReferenceMatchesDefinition(gcnew AssemblyName(args->Name), name)) {
            return Assembly::Load(name);
        }
    }
    return nullptr;
}

// register a custom assembly load handler
void LoadAssembly()
{
    System::AppDomain::CurrentDomain->AssemblyResolve +=
        gcnew System::ResolveEventHandler(OnAssemblyResolve);
}

class SharpLibWrapper : public SharpLibHelper {
private:
    gcroot<SharpClass^> _sharpClass;

public:
    SharpLibWrapper()
    {
        _sharpClass = gcnew SharpClass();
    }

    virtual bool DownloadUrl(std::string& url, std::string& err, std::string& result)
    {
        try
        {
            System::String^ data =  _sharpClass->DownloadUrl(gcnew System::String(url.c_str()));

            System::IntPtr p = Marshal::StringToHGlobalAnsi(data);
            result = static_cast<char*>(p.ToPointer());
            Marshal::FreeHGlobal(p);
            return false;
        }
        catch(System::Exception^ e)
        {
            System::IntPtr p = Marshal::StringToHGlobalAnsi(e->Message);
            err = static_cast<char*>(p.ToPointer());
            Marshal::FreeHGlobal(p);
            return true;
        }
    }
    
    virtual int GetNumber()
    {
        return _sharpClass->Number;
    }
};

SharpLibHelper* SharpLibHelper::New()
{
    return new SharpLibWrapper();
}

