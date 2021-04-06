#include "fakekms/cpp/fakekms.h"

#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "gmock/gmock.h"
#include "grpcpp/create_channel.h"
#include "grpcpp/security/credentials.h"

namespace fakekms {
namespace {

namespace kms_v1 = ::google::cloud::kms::v1;

// Declare an IsOk matcher locally so that we don't have to depend on kmsp11.
MATCHER(IsOk, absl::StrFormat("status is %sOK", negation ? "not " : "")) {
  return arg.ok();
}

TEST(ServerTest, SmokeTest) {
  absl::StatusOr<std::unique_ptr<Server>> server = Server::New();
  ASSERT_THAT(server.status(), IsOk());

  std::unique_ptr<kms_v1::KeyManagementService::Stub> stub =
      (*server)->NewClient();

  grpc::ClientContext ctx;

  kms_v1::CreateKeyRingRequest req;
  req.set_parent("projects/my-project/locations/us-central1");
  req.set_key_ring_id("kr1");

  kms_v1::KeyRing kr;

  EXPECT_THAT(stub->CreateKeyRing(&ctx, req, &kr), IsOk());
  EXPECT_EQ(kr.name(),
            "projects/my-project/locations/us-central1/keyRings/kr1");
}

}  // namespace
}  // namespace fakekms
