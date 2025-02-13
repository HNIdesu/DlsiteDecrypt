def parse_version(version):
    major,minor = version.split(".")
    return int(major)*100+int(minor)