import yaml

try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper  # type: ignore

import datetime


class Undefined:
    """
    Represents missing values in YAML (as opposed to null which is None in Python)
    """

    pass


def mapping(tag):
    """
    A decorator which allows for serializing/deserializing a class as a YAML mapping (dictionary).
    The class must be able to be constructed with `**kwargs` to set its data.

    :param tag: The tag to use for the type (e.g. `!MyTag`)
    :type tag: str
    """

    def decorator(obj_class):
        def construct(loader, node):
            try:
                attrs = loader.construct_mapping(node)
                instance = obj_class(**attrs)
                return instance
            except yaml.constructor.ConstructorError:
                return obj_class()

        def represent(dumper, data):
            attrs = {
                key: value
                for key, value in data.__dict__.items()
                if value is not Undefined
            }
            return dumper.represent_mapping(tag, attrs)

        yaml.add_constructor(tag, construct, Loader=Loader)
        yaml.add_representer(obj_class, represent, Dumper=Dumper)
        return obj_class

    return decorator


def sequence(tag):
    """
    A decorator which allows for serializing/deserializing a class as a YAML sequence (list).
    The class must be able to be constructed with `*args` to set it the items, and also be iterable.

    :param tag: The tag to use for the type (e.g. `!MyTag`)
    :type tag: str
    """

    def decorator(obj_class):
        def construct(loader, node):
            try:
                args = loader.construct_sequence(node)
                return obj_class(*args)
            except yaml.constructor.ConstructorError:
                return obj_class()

        def represent(dumper, data):
            return dumper.represent_sequence(tag, iter(data))

        yaml.add_constructor(tag, construct, Loader=Loader)
        yaml.add_representer(obj_class, represent, Dumper=Dumper)
        return obj_class

    return decorator


def scalar(tag):
    """
    A decorator which allows for serializing/deserializing a class as a YAML scalar (string).
    The class must be able to be constructed with a string argument, and implement `get_value()`
    to return the string to be serialized.

    :param tag: The tag to use for the type (e.g. `!MyTag`)
    :type tag: str
    """

    def decorator(obj_class):
        def construct(loader, node):
            value = loader.construct_scalar(node)
            return obj_class(value)

        def represent(dumper, data):
            return dumper.represent_scalar(tag, str(data.get_value()))

        yaml.add_constructor(tag, construct, Loader=Loader)
        yaml.add_representer(obj_class, represent, Dumper=Dumper)
        return obj_class

    return decorator


def load(stream):
    """
    Helper function which loads YAML
    """
    return yaml.load(stream, Loader=Loader)


def load_all(stream):
    """
    Helper function which loads YAML as a list of documents
    """
    return yaml.load_all(stream, Loader=Loader)


def dump(data):
    """
    Helper function which serializes objects as YAML
    """
    return yaml.dump(data, Dumper=Dumper)


def dump_all(data):
    """
    Helper function which serializes a list of objects as YAML
    """
    return yaml.dump_all(data, Dumper=Dumper)


def range_representer(dumper, data):
    """
    A YAML `!Range l..u` tag
    """
    scalar = u"{}..{}".format(data.start, data.stop - 1)
    return dumper.represent_scalar(u"!Range", scalar)


yaml.add_representer(range, range_representer, Dumper=Dumper)


def range_constructor(loader, node):
    """
    A YAML `!Range l..u` tag
    """
    value = loader.construct_scalar(node)
    a, b = map(int, value.split(".."))
    return range(a, b + 1)


yaml.add_constructor(u"!Range", range_constructor, Loader=Loader)


def dt_representer(dumper, data):
    """
    A YAML `!Duration` tag
    """
    scalar = u"{}ms".format(data.total_seconds() * 1000)
    return dumper.represent_scalar(u"!Duration", scalar)


yaml.add_representer(datetime.timedelta, dt_representer, Dumper=Dumper)


def dt_constructor(loader, node):
    """
    A YAML `!Duration` tag
    """
    value = loader.construct_scalar(node)
    if value.endswith("us"):
        return datetime.timedelta(microseconds=float(value[:-2]))
    if value.endswith("ms"):
        return datetime.timedelta(milliseconds=float(value[:-2]))
    if value.endswith("s"):
        return datetime.timedelta(seconds=float(value[:-1]))


yaml.add_constructor(u"!Duration", dt_constructor, Loader=Loader)


def list_representer(dumper, data):
    """
    Changes flow style of lists to be on a single line if only primitives are contained
    """
    flow_style = all(isinstance(i, (int, float, str, bool)) for i in data)
    return dumper.represent_sequence(
        "tag:yaml.org,2002:seq", data, flow_style=flow_style
    )


yaml.add_representer(list, list_representer)
