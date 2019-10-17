from conans import ConanFile, CMake, tools


class DataCollectorConan(ConanFile):
    name = "data-collector"
    version = "0.1.0"
    license = "MIT"
    author = "Amnon Paz <pazamnon@gmail.com>"
    url = "https://github.com/amnonpaz/iot"
    description = "MqTT client for contorlling remote endpoints"
    topics = ("iot", "mqtt")
    settings = "os", "compiler", "build_type", "arch"
    options = {"local": [True, False]}
    default_options = {"local": False}
    generators = [ "cmake", "cmake_find_package" ]
    requires = [ "mosquitto/1.4.15@bincrafters/stable" ]
    agent_target = "collector-agent"

    def get_source_folder(self):
        if not self.options.local:
            return "iot/server-side/data-collector"
        return ""

    def source(self):
        self.run("git clone https://github.com/amnonpaz/iot.git")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_PROJECT_NAME"] = self.name
        cmake.definitions["PROJECT_VERSION"] = self.version
        cmake.definitions["PROJECT_DESCRIPTION"] = self.description
        cmake.definitions["CMAKE_CXX_STANDARD"] = 17
        cmake.definitions["AGENT_TARGET"] = self.agent_target

        cmake.configure(source_folder=self.get_source_folder())
        cmake.build()

    def package(self):
        self.copy(self.agent_target, "bin", "build/bin")

    def package_info(self):
        self.cpp_info.libs = ["data-collector"]

