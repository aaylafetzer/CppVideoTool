#include <string>
#include <opencv2/opencv.hpp>
#include <argparse/argparse.hpp>
#include <utility>

class Filter {
private:
    std::string variable;
    std::string value;
public:
    Filter(std::string  variable, std::string  value) : variable(std::move(variable)), value(std::move(value)) {}
    std::string const& getValue() {
        return value;
    }
    bool variableIs(const std::string& test) {
        return test == variable;
    }
};

std::vector<Filter> parseFilterString(std::string filterString) {
    std::vector<Filter> output;
    std::vector<std::string> buffer;

    // Iterate through the given filter string and split at the delimiter
    // Source: https://stackoverflow.com/a/14266139/13026048
    size_t pos;
    while ((pos = filterString.find(';')) != std::string::npos) {
        buffer.push_back(filterString.substr(0, pos));
        filterString.erase(0, pos + 1);
    }
    buffer.push_back(filterString);
    // Convert the strings in buffer to Filter type
    for (const std::string& i : buffer) {
        Filter newFilter(
                i.substr(0, i.find('=')),
                i.substr(i.find('=')+1, i.length())
        );
        output.push_back(newFilter);
    }
    return output;
}

int main(int argc, char *argv[] )
{
    // Argument Parser
    argparse::ArgumentParser args("VideoTool");
    args.add_argument("output")
        .help("output file");
    args.add_argument("--filter")
        .help("string of filters");
    args.add_argument("inputs")
        .help("input files")
        .remaining();
    args.parse_args(argc, argv);

    auto inputFiles = args.get<std::vector<std::string>>("inputs");
    auto outFile (args.get<std::string>("output"));

    // Get first file in inputs
    cv::VideoCapture firstFile = cv::VideoCapture(inputFiles[0]);
    auto firstFileFrameRate = (float)firstFile.get(cv::CAP_PROP_FPS);
    int firstFileFrameWidth = (int)firstFile.get(cv::CAP_PROP_FRAME_WIDTH);
    int firstFileFrameHeight = (int)firstFile.get(cv::CAP_PROP_FRAME_HEIGHT);

    // Default filters
    float outputFrameRate = firstFileFrameRate;
    int outputResolution [2] = {firstFileFrameWidth, firstFileFrameHeight};
    int slice [4] = {0, 0, firstFileFrameWidth, firstFileFrameHeight};

    // Get updated values from filters
    if (args.present("--filter")) {
        std::vector<Filter> filters = parseFilterString(args.get("--filter"));
        for (Filter i : filters) {
            if (i.variableIs("fps")) { outputFrameRate = std::stoi(i.getValue()); }
            if (i.variableIs("rescale")) {
                char* j = const_cast<char*>(i.getValue().c_str());
                int newRes[2] = {std::stoi(std::strtok(j, "x")),
                                 std::stoi(std::strtok(nullptr, "x"))};
                std::memcpy(outputResolution, newRes, sizeof(outputResolution));
            }
            if (i.variableIs("slice")) {
                char* j = const_cast<char*>(i.getValue().c_str());
                long int a[] = {std::stoi(strtok(j, ",")),
                                std::stoi(strtok(nullptr, ":")),
                                std::stoi(strtok(nullptr, ",")),
                                std::stoi(strtok(nullptr, ","))};
                slice[0] = a[0];
                slice[1] = a[1];
                slice[2] = a[2];
                slice[3] = a[3];
            }
        }
    }


    // Main loop
    cv::VideoWriter output = cv::VideoWriter(outFile,
                                             cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                             outputFrameRate, cv::Size(outputResolution[0], outputResolution[1]));
    for (const auto& x : inputFiles) {
        // Load video
        auto video = cv::VideoCapture(x);
        // Get video information
        int frameWidth = (int)video.get(cv::CAP_PROP_FRAME_WIDTH);
        int frameHeight = (int)video.get(cv::CAP_PROP_FRAME_HEIGHT);
        while (video.isOpened()) {
            // Load video frame
            cv::Mat frame;
            video >> frame;
            if (frame.empty()) {
                video.release();
                break;
            }
            // Frame processing
            // Resize frame if different from output resolution
            cv::Mat intermediateFrame;
            if (frameWidth != outputResolution[0] or frameHeight != outputResolution[1]) {
                cv::resize(frame, intermediateFrame, cv::Size(outputResolution[0], outputResolution[1]));
                frame = intermediateFrame;
            }
            // ROI
            intermediateFrame = frame;
            frame = intermediateFrame(cv::Range(slice[0], slice[2]),
                          cv::Range(slice[1], slice[3]));

            // Resize frame to output resolution if necessary
            if (slice[2] - slice[0] != outputResolution[0] or slice[3] - slice[1] != outputResolution[1]) {
                intermediateFrame = frame;
                cv::resize(intermediateFrame, frame, cv::Size(outputResolution[0], outputResolution[1]));
            }
            // Write frame
            output.write(frame);
        }
    }
    return 0;
}