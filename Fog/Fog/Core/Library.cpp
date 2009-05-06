// [Fog/Core Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Precompiled headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Error.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/Vector.h>

#if defined(FOG_LIBRARY_WINDOWS)
#include <windows.h>
#endif // FOG_LIBRARY_WINDOWS

#if defined(FOG_LIBRARY_DL)
#include <dlfcn.h>
#include <errno.h>
#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#endif // FOG_LIBRARY_DL

namespace Fog {

// ============================================================================
// [Fog::Library - Local]
// ============================================================================

struct Library_Local
{
  Lock lock;
  Vector<String32> paths;
};

static Static<Library_Local> library_local;

// ============================================================================
// [Fog::Library - Platform support]
// ============================================================================

#if defined(FOG_LIBRARY_WINDOWS)
static err_t platformOpenLibrary(const String32& fileName, void** handle)
{
  err_t err;
  TemporaryString16<TemporaryLength> fileNameW;
  if ( (err = fileNameW.set(fileName)) ) return err;

  HMODULE hLibrary = ::LoadLibraryW(fileNameW.cStrW());
  if (!hLibrary) return GetLastError();

  *handle = (void*)hLibrary;
  return Error::Ok;
}

static void platformCloseLibrary(void* handle)
{
  ::FreeLibrary((HMODULE)handle);
}

static void* platformLoadSymbol(void* handle, const char* symbol)
{
  if (handle == NULL) return NULL;
  return (void*)GetProcAddress((HINSTANCE)handle, symbol);
}
#endif // FOG_LIBRARY_WINDOWS

#if defined(FOG_LIBRARY_DL)
static err_t platformOpenLibrary(const String32& fileName, void** handle)
{
  err_t err;
  TemporaryString8<TemporaryLength> fileName8;
  if ( (err = fileName8.set(fileName, TextCodec::local8())) ) return err;

  if ((*handle = (void*)dlopen(fileName8.cStr(), RTLD_NOW)) == NULL)
  {
    return Error::LibraryOpenFailed;
  }
  else
  {
    return Error::Ok;
  }
}

static void platformCloseLibrary(void* handle)
{
  dlclose(handle);
}

static void* platformLoadSymbol(void* handle, const char* symbol)
{
  if (handle == NULL) return NULL;
  return (void *)dlsym(handle, symbol);
}
#endif // FOG_LIBRARY_DL

// ============================================================================
// [Fog::Library]
// ============================================================================

const Char8* Library::systemPrefix;
const Char8* Library::systemSuffix;
const Char8* Library::systemExtension;

Library::Library() :
  _d(sharedNull->refAlways())
{
}

Library::Library(const Library& other) :
  _d(other._d->ref())
{
}

Library::Library(const String32& fileName, uint32_t openFlags) :
  _d(sharedNull->refAlways())
{
  open(fileName, openFlags);
}

Library::~Library()
{
  _d->deref();
}

err_t Library::open(const String32& _fileName, uint32_t openFlags)
{
  err_t err;
  TemporaryString32<TemporaryLength> fileName;
  
  if ( (err = fileName.set(_fileName))) return err;

  if ((openFlags & OpenSystemPrefix) != 0)
  {
    if ( (err = fileName.insert(
      fileName.lastIndexOf(Char32('/')) + 1,
      StubAscii8(systemPrefix))) ) return err;
  }
  if ((openFlags & OpenSystemSuffix) != 0)
  {
    if ( (err = fileName.append(
      StubAscii8(systemSuffix))) ) return err;
  }

  void* handle;
  if ( (err = platformOpenLibrary(fileName, &handle)) ) return err;

  Data* newd = Data::alloc();
  if (!newd)
  {
    platformCloseLibrary(handle);
    return Error::OutOfMemory;
  }
  newd->handle = handle;

  AtomicBase::ptr_setXchg(&_d, newd)->deref();
  return Error::Ok;
}

err_t Library::openPlugin(const String32& category, const String32& fileName)
{
  err_t err;
  TemporaryString32<TemporaryLength> relative;
  TemporaryString32<TemporaryLength> absolute;

  if ( (err = relative.append(StubAscii8(systemPrefix))) ) return err;
  if (!category.isEmpty())
  {
    if ( (err = relative.append(category)) ) return err;
    if ( (err = relative.append(Char32('_'))) ) return err;
  }
  if ( (err = relative.append(fileName)) ) return err;
  if ( (err = relative.append(StubAscii8(systemSuffix))) ) return err;

  if (FileSystem::findFile(paths(), relative, absolute))
    return open(absolute, 0);
  else
    return Error::FileNotExists;
}

void Library::close()
{
  AtomicBase::ptr_setXchg(&_d, sharedNull->refAlways())->deref();
}

void* Library::symbol(const char* symbolName)
{
  return platformLoadSymbol(_d->handle, symbolName);
}

void* Library::symbol(const String32& symbolName)
{
  TemporaryString8<TemporaryLength> symb8;
  symb8.append(StubUtf32(symbolName), TextCodec::utf8());
  return platformLoadSymbol(_d->handle, symb8.cStr());
}

sysuint_t Library::symbols(void** target, const char* symbols, sysuint_t symbolsLength, sysuint_t symbolsCount, char** fail)
{
  sysuint_t i;

  void* address;
  void* handle = _d->handle;
  const char* current;
  const char* symbolsCur = symbols;
  const char* symbolsEnd = symbols + symbolsLength;

  if (fail) *fail = NULL;

  for (i = 0; i != symbolsCount; i++)
  {
    current = symbolsCur;
    for (;;)
    {
      if (symbolsCur == symbolsEnd) goto end;
      if (*symbolsCur++ == 0) break;
    }

    if ((address = platformLoadSymbol(handle, current)) == NULL)
    {
      if (fail) *fail = (char*)current;
      goto end;
    }

    target[i] = address;
  }

end:
  return i;
}

// [Operator Overload]

Library& Library::operator=(const Library& other)
{
  AtomicBase::ptr_setXchg(&_d, sharedNull->refAlways())->deref();
  return *this;
}

// [Paths]

Vector<String32> Library::paths()
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths;
}

bool Library::addPath(const String32& path)
{
  AutoLock locked(library_local.instance().lock);
  if (library_local.instance().paths.indexOf(path) == InvalidIndex)
  {
    library_local.instance().paths.append(path);
    return true;
  }
  else
  {
    return false;
  }
}

bool Library::removePath(const String32& path)
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths.remove(path) != 0;
}

bool Library::hasPath(const String32& path)
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths.indexOf(path) != InvalidIndex;
}

// ============================================================================
// [Fog::Library::Data]
// ============================================================================

Static<Library::Data> Library::sharedNull;

Library::Data* Library::Data::ref() const
{
  return refAlways();
}

void Library::Data::deref()
{
  if (refCount.deref())
  {
    if (handle) platformCloseLibrary(handle);
    Memory::free(this);
  }
}

Library::Data* Library::Data::alloc()
{
  Data* d = (Data*)Memory::alloc(sizeof(Data));
  if (!d) return NULL;

  d->refCount.init(1);
  d->handle = NULL;

  return d;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_library_init(void)
{
  Fog::Library::sharedNull.init();
  Fog::Library::Data* d = Fog::Library::sharedNull.instancep();
  d->refCount.init(1);
  d->handle = NULL;

  Fog::library_local.init();

#if defined(FOG_OS_WINDOWS)
  static const char libraryPrefix[] = "";
  static const char librarySuffix[] = ".dll";
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
  static const char libraryPrefix[] = "lib";
  static const char librarySuffix[] = ".so";
#endif // FOG_OS_POSIX

  Fog::Library::systemPrefix = (const Fog::Char8*)libraryPrefix;
  Fog::Library::systemSuffix = (const Fog::Char8*)librarySuffix;
  Fog::Library::systemExtension = (const Fog::Char8*)librarySuffix + 1;

  // Application directory usually contains plugins and library itself under
  // Windows, but we will add it also for posix OSes.

  // TODO: Get application directory instead of working directory
  Fog::String32 workingDirectory;
  Fog::FileSystem::getWorkingDirectory(workingDirectory);
  Fog::library_local.instance().paths.append(workingDirectory);

#if defined(FOG_OS_POSIX)
#if FOG_ARCH_BITS == 32
  Fog::library_local.instance().paths.append(Fog::StubAscii8("/usr/lib"));
  Fog::library_local.instance().paths.append(Fog::StubAscii8("/usr/local/lib"));
#else
  Fog::library_local.instance().paths.append(Fog::StubAscii8("/usr/lib64"));
  Fog::library_local.instance().paths.append(Fog::StubAscii8("/usr/local/lib64"));
#endif // FOG_ARCH_BITS
#endif // FOG_OS_POSIX

  return Error::Ok;
}

FOG_INIT_DECLARE void fog_library_shutdown(void)
{
  Fog::library_local.destroy();

  Fog::Library::sharedNull.instancep()->refCount.dec();
  Fog::Library::sharedNull.destroy();
}
