/**
 *    Copyright (C) 2018-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/platform/basic.h"

#include "mongo/db/s/type_shard_identity.h"

#include "mongo/base/status_with.h"
#include "mongo/db/jsobj.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
namespace {

using std::string;

TEST(ShardIdentityType, RoundTrip) {
    auto clusterId(OID::gen());
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "shardName"
                    << "s1"
                    << "clusterId"
                    << clusterId
                    << "configsvrConnectionString"
                    << "test/a:123");

    auto result = ShardIdentityType::fromShardIdentityDocument(doc);
    ASSERT_OK(result.getStatus());

    auto shardIdentity = result.getValue();
    ASSERT_EQ("test/a:123", shardIdentity.getConfigsvrConnectionString().toString());
    ASSERT_EQ("s1", shardIdentity.getShardName());
    ASSERT_EQ(clusterId, shardIdentity.getClusterId());

    ASSERT_BSONOBJ_EQ(doc, shardIdentity.toShardIdentityDocument());
}

TEST(ShardIdentityType, ParseMissingId) {
    auto doc = BSON("configsvrConnectionString"
                    << "test/a:123"
                    << "shardName"
                    << "s1"
                    << "clusterId"
                    << OID::gen());

    auto result = ShardIdentityType::fromShardIdentityDocument(doc);
    ASSERT_NOT_OK(result.getStatus());
}

TEST(ShardIdentityType, ParseMissingConfigsvrConnString) {
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "shardName"
                    << "s1"
                    << "clusterId"
                    << OID::gen());

    auto result = ShardIdentityType::fromShardIdentityDocument(doc);
    ASSERT_NOT_OK(result.getStatus());
}

TEST(ShardIdentityType, ParseMissingShardName) {
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "configsvrConnectionString"
                    << "test/a:123"
                    << "clusterId"
                    << OID::gen());

    auto result = ShardIdentityType::fromShardIdentityDocument(doc);
    ASSERT_NOT_OK(result.getStatus());
}

TEST(ShardIdentityType, ParseMissingClusterId) {
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "configsvrConnectionString"
                    << "test/a:123"
                    << "shardName"
                    << "s1");

    auto result = ShardIdentityType::fromShardIdentityDocument(doc);
    ASSERT_NOT_OK(result.getStatus());
}

TEST(ShardIdentityType, InvalidConnectionString) {
    auto clusterId(OID::gen());
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "configsvrConnectionString"
                    << "test/,,,"
                    << "shardName"
                    << "s1"
                    << "clusterId"
                    << clusterId);

    ASSERT_EQ(ErrorCodes::FailedToParse,
              ShardIdentityType::fromShardIdentityDocument(doc).getStatus());
}

TEST(ShardIdentityType, NonReplSetConnectionString) {
    auto clusterId(OID::gen());
    auto doc = BSON("_id"
                    << "shardIdentity"
                    << "configsvrConnectionString"
                    << "local:123"
                    << "shardName"
                    << "s1"
                    << "clusterId"
                    << clusterId);

    ASSERT_EQ(ErrorCodes::UnsupportedFormat,
              ShardIdentityType::fromShardIdentityDocument(doc).getStatus());
}

TEST(ShardIdentityType, CreateUpdateObject) {
    auto updateObj = ShardIdentityType::createConfigServerUpdateObject("test/a:1,b:2");
    auto expectedObj = BSON("$set" << BSON("configsvrConnectionString"
                                           << "test/a:1,b:2"));
    ASSERT_BSONOBJ_EQ(expectedObj, updateObj);
}

}  // namespace mongo
}  // unnamed namespace
