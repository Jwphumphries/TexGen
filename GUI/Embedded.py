# This file was automatically generated by SWIG (http://www.swig.org).
# Version 4.0.0
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info < (2, 7, 0):
    raise RuntimeError('Python 2.7 or later required')

# Import the low-level C/C++ module
if __package__ or '.' in __name__:
    from . import _Embedded
else:
    import _Embedded

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if name == "thisown":
        return self.this.own(value)
    if name == "this":
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if not static:
        object.__setattr__(self, name, value)
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if name == "thisown":
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


def _swig_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "thisown":
            self.this.own(value)
        elif name == "this":
            set(self, name, value)
        elif hasattr(self, name) and isinstance(getattr(type(self), name), property):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add instance attributes to %s" % self)
    return set_instance_attr


def _swig_setattr_nondynamic_class_variable(set):
    def set_class_attr(cls, name, value):
        if hasattr(cls, name) and not isinstance(getattr(cls, name), property):
            set(cls, name, value)
        else:
            raise AttributeError("You cannot add class attributes to %s" % cls)
    return set_class_attr


def _swig_add_metaclass(metaclass):
    """Class decorator for adding a metaclass to a SWIG wrapped class - a slimmed down version of six.add_metaclass"""
    def wrapper(cls):
        return metaclass(cls.__name__, cls.__bases__, cls.__dict__.copy())
    return wrapper


class _SwigNonDynamicMeta(type):
    """Meta class to enforce nondynamic attributes (no new attributes) for a class"""
    __setattr__ = _swig_setattr_nondynamic_class_variable(type.__setattr__)


class SwigPyIterator(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _Embedded.delete_SwigPyIterator

    def value(self):
        return _Embedded.SwigPyIterator_value(self)

    def incr(self, n=1):
        return _Embedded.SwigPyIterator_incr(self, n)

    def decr(self, n=1):
        return _Embedded.SwigPyIterator_decr(self, n)

    def distance(self, x):
        return _Embedded.SwigPyIterator_distance(self, x)

    def equal(self, x):
        return _Embedded.SwigPyIterator_equal(self, x)

    def copy(self):
        return _Embedded.SwigPyIterator_copy(self)

    def next(self):
        return _Embedded.SwigPyIterator_next(self)

    def __next__(self):
        return _Embedded.SwigPyIterator___next__(self)

    def previous(self):
        return _Embedded.SwigPyIterator_previous(self)

    def advance(self, n):
        return _Embedded.SwigPyIterator_advance(self, n)

    def __eq__(self, x):
        return _Embedded.SwigPyIterator___eq__(self, x)

    def __ne__(self, x):
        return _Embedded.SwigPyIterator___ne__(self, x)

    def __iadd__(self, n):
        return _Embedded.SwigPyIterator___iadd__(self, n)

    def __isub__(self, n):
        return _Embedded.SwigPyIterator___isub__(self, n)

    def __add__(self, n):
        return _Embedded.SwigPyIterator___add__(self, n)

    def __sub__(self, *args):
        return _Embedded.SwigPyIterator___sub__(self, *args)
    def __iter__(self):
        return self

# Register SwigPyIterator in _Embedded:
_Embedded.SwigPyIterator_swigregister(SwigPyIterator)


def GetRenderWindow(*args):
    return _Embedded.GetRenderWindow(*args)

def CreateRenderWindow(*args):
    return _Embedded.CreateRenderWindow(*args)

def DeleteRenderWindow(*args):
    return _Embedded.DeleteRenderWindow(*args)

def Refresh(*args):
    return _Embedded.Refresh(*args)

def ReceivePythonOutput(Output, Type):
    return _Embedded.ReceivePythonOutput(Output, Type)


