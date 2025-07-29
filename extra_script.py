import os
Import("env")

# ツールチェーンパスも含めたい場合
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# 出力先をプロジェクトルートに指定
env.Replace(COMPILATIONDB_PATH=os.path.join("$PROJECT_DIR", "compile_commands.json"))
