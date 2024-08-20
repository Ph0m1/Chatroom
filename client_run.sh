PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BUILD_DIR="${PROJECT_DIR}/build"

IP="127.0.0.1"
PORT="9098"

while [["$#" -gt 0]];do
    case $1 in
        -i|--ip) IP="$2"; shift ;;
        -p|--port) PORT="$2"; shift ;;
        *) echo "未知参数： $1"; exit 1;;
    esac
    shift
done

echo "IP: $IP"
echo "PORT: $PORT"

cd "$PROJECT_DIR"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "开始编译..."
cmake ..

echo "正在构建项目..."
make client -j

if [ $? -eq 0 ]; then
    echo "构建成功"
    echo "正在运行客户端..."
    ./client $IP $PORT
else
    echo "构建失败"
    exit 1
fi

