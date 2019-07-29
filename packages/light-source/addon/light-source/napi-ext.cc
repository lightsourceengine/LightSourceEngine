/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napi-ext.h"

using Napi::Error;

namespace ls {

AsyncWork::AsyncWork(Napi::Env env, const std::string& resourceName, ExecuteFunction execute, CompleteFunction complete)
    : execute(execute), complete(complete), env(env) {
    napi_value resourceId;
    napi_status status = napi_create_string_latin1(
        env,
        resourceName.c_str(),
        NAPI_AUTO_LENGTH,
        &resourceId);

    NAPI_THROW_IF_FAILED_VOID(env, status);

    status = napi_create_async_work(
        env,
        nullptr,
        resourceId,
        AsyncWork::OnExecute,
        AsyncWork::OnComplete,
        this,
        &this->work);

    NAPI_THROW_IF_FAILED_VOID(env, status);

    status = napi_queue_async_work(env, this->work);

    if (status != napi_ok) {
        napi_delete_async_work(env, this->work);
        this->work = nullptr;
    }

    NAPI_THROW_IF_FAILED_VOID(env, status);
}

AsyncWork::~AsyncWork() {
    if (this->work) {
        napi_delete_async_work(this->env, this->work);
        this->work = nullptr;
    }
}

void AsyncWork::Cancel() {
    if (this->work) {
        napi_cancel_async_work(this->env, this->work);
    }
}

void AsyncWork::OnExecute(napi_env env, void* self) {
    auto asyncWork{ static_cast<AsyncWork*>(self) };

    try {
        asyncWork->execute(asyncWork->env);
        asyncWork->result = napi_ok;
    } catch (std::exception& e) {
        asyncWork->errorMessage = e.what();
        asyncWork->result = napi_generic_failure;
    }
}

void AsyncWork::OnComplete(napi_env env, napi_status status, void* self) {
    auto asyncWork{ static_cast<AsyncWork*>(self) };

    asyncWork->complete(asyncWork->env, asyncWork->result, asyncWork->errorMessage);
}

} // namespace ls
