#ifndef MESSAGEKEY_H
#define MESSAGEKEY_H
static constexpr auto APP_START_MSG = "EtrekStarted";
static constexpr auto APP_START_SHOTDOWN_MSG = "StartEtrekShotDown";
static constexpr auto APP_SHOTDOWN_SUCCEED_MSG = "EtrekShotdownSucceed";

static constexpr auto AUTH_USER_CREATION_SUCCEED_MSG = "UserCreationSucceed";
static constexpr auto AUTH_USER_CREATION_FAILED_ERROR = "UserCreationFailedError";
static constexpr auto AUTH_USER_UPDATE_FAILED_ERROR = "UserUpdateFailedError";
static constexpr auto AUTH_USER_UPDATE_SUCCEED_MSG = "UserUpdateSucceed";
static constexpr auto AUTH_USER_CREATION_SUCCEED_TITLE_MSG = "UserCreationSuccedTitle";
static constexpr auto AUTH_USER_CREATION_FAILED_TITLE_MSG = "UserCreationFailedTitle";
static constexpr auto AUTH_USER_UPDATE_FAILED_TITLE_MSG = "UserUpdateFailedTitle";
static constexpr auto AUTH_USER_UPDATE_SUCCEED_TITLE_MSG = "UserUpdateSucceedTitle";
static constexpr auto AUTH_CHECK_USER_EXISTS_FAILED_MSG = "CheckUserExistsFailed";
static constexpr auto AUTH_USERNAME_ALREADY_EXISTS_MSG = "UsernameAlreadyExists";

static constexpr auto AUTH_USER_DELETE_FAILED_ERROR = "UserDeleteFailedError";
static constexpr auto AUTH_USER_DELETE_SUCCEED_TITLE_MSG = "UserDeleteSucceedTitle";
static constexpr auto AUTH_USER_DELETE_SUCCEED_MSG = "UserDeleteSucceed";

static constexpr auto AUTH_USER_NOT_EXISTS_ERROR = "UserNotExist";
static constexpr auto AUTH_LOGIN_CANCELED_BY_USR_MSG = "LoginCanceledByUser";
static constexpr auto AUTH_VALIDIATION_FAILED_TITLE_MSG = "ValidationFailedTitle";
static constexpr auto AUTH_EMPTY_USER_NAME_WARNING_MSG = "EmptyUserNameWarning";
static constexpr auto AUTH_USER_ALREADY_EXISTS_WARNING_MSG ="UserAlreadyExistsWarning";
static constexpr auto AUTH_EMPTY_PASSWORD_WARNING_MSG = "EmptyPasswordWarning";
static constexpr auto AUTH_PASSWORD_NOT_MATCHING_WARNING_MSG = "PasswordNotMatchingWarning";
static constexpr auto AUTH_PASSWORD_COMPLEXITY_WARNING_MSG = "PasswordComplexityMatchingWarning";
static constexpr auto AUTH_ROLE_NOT_SELECTED_WARNING_MSG = "RoleNotSekectedWarning";
static constexpr auto AUTH_SELECT_ONE_FOR_EDIT_MSG = "SelectOneForEdit";
static constexpr auto AUTH_PLEASE_SELECT_ONE_MSG = "PleaseSelectOne";
static constexpr auto AUTH_FAILED_TO_CREATE_USER_ERROR_MSG = "FailedToCreateUserError";
static constexpr auto AUTH_FAILED_TO_UPDATE_USER_ERROR_MSG = "FailedToUpdateUserError";
static constexpr auto AUTH_FAILED_TO_DELETE_USER_ERROR_MSG = "FailedToDeleteUserError";
static constexpr auto AUTH_INVALID_ROLE_ID_ERROR_MSG = "InvalidRoleID";
static constexpr auto AUTH_ROLE_ASSIGNMENT_FAILED_ERROR_MSG = "RoleAssignmentFailed";
static constexpr auto AUTH_CHECK_USERNAME_FAILED_ERROR = "CheckUsernameFailed";
static constexpr auto AUTH_CLEAR_EXISTING_ROLES_FAILED_ERROR_MSG = "FailedToClearExistingRoles";
static constexpr auto AUTH_FAILED_TO_CREATE_ROLE_ERROR_MSG = "FailedToCreateRoleError";
static constexpr auto AUTH_ROLE_ASSIGNMENT_SUCCEED_MSG = "RoleAssignmentSucceed";
static constexpr auto AUTH_FAILED_TO_REMOVE_ROLE_ERROR_MSG = "FailedToRemoveRoleError";
static constexpr auto AUTH_CHECK_ROLE_FAILED_ERROR_MSG = "CheckRoleFailed";
static constexpr auto AUTH_GET_USER_FAILED_ERROR_MSG = "GetUserFailed";

static constexpr auto AUTH_START_INIT_AUTH_MSG = "StartAuthenticationInit";
static constexpr auto AUTH_INIT_AUTH_SUCCEED = "AuthenticationInitSucceed";
static constexpr auto AUTH_START_MSG = "StartAuthentication";
static constexpr auto AUTH_SUCCEED = "AuthenticationSucceed";

static constexpr auto UNEXPECTED_ERROR_OCCURED_ERROR_MSG = "UnexpectedErrorOccurred";
static constexpr auto QUERY_FAILED_ERROR_MSG = "QueryFailed";


static constexpr auto SQL_SCRIPT_EXECUTION_FAILED_ERROR_MSG = "SqlScriptExecutionFailedError";
static constexpr auto SQL_SCRIPT_EXECUTION_SUCCEED_MSG = "SqlScriptExecutionSucceed";
static constexpr auto SQL_SCRIPT_NO_TABLES_FOUND_MSG = "NoTablesFoundRunningSetupScript";
static constexpr auto SQL_SCRIPT_FAILED_TO_OPEN_MSG = "FailedToOpenSetupMySQLScript";

static constexpr auto DB_START_INIT_MSG = "StartDatabaseInit";
static constexpr auto DB_INIT_SUCCESS_MSG = "DatabaseInitSuccess";
static constexpr auto DB_FAILED_TO_CREATE_MSG = "FailedToCreateDatabase";
static constexpr auto DB_FAILED_TO_CONNECT_OR_CREATE_MSG = "FailedToCreateOrConnectToTheDatabase";
static constexpr auto DB_FAILED_TO_OPEN_AFTER_CREATION_MSG = "FailedToOpenDatabaseAfterCreation";
static constexpr auto DB_CANNOT_CONNECT_TO_MYSQL_MSG = "CanNotConnectToMySQLSystemDB";


static constexpr auto RIS_START_NETWORK_INIT_MSG = "RisStartNetworkInitialization";
static constexpr auto RIS_NETWORK_INIT_FAILED = "RisNetworkInitializationFailed";
static constexpr auto RIS_NETWORK_INIT_SUCCEED = "RisNetworkInitializationSucceed";
static constexpr auto RIS_ASSOCIATION_NEGOTIATION_FAILED = "RisAssociationNegotiationFailed";
static constexpr auto RIS_ASSOCIATION_NEGOTIATION_SUCCEED = "RisAssociationNegotiationSucceed";
static constexpr auto RIS_ADD_PRESENTATION_CONTEXT_FAILED = "RisAddPresentationContextFailed";
static constexpr auto RIS_ADD_PRESENTATION_CONTEXT_SUCCEED = "RisAddPresentationContextSucceed";
static constexpr auto RIS_C_ECHO_FAILED = "RisCEchoFailed";
static constexpr auto RIS_C_ECHO_SUCCEED = "RisCEchoSucceed";
static constexpr auto RIS_RELEASE_CONNECTION_FAILED = "RisReleaseConnectionFailed";
static constexpr auto RIS_RELEASE_CONNECTION_SUCCEED = "RisReleaseConnectionSucceed";
static constexpr auto RIS_C_FIND_FAILED = "RisCFindFailed";
static constexpr auto RIS_CONNECTION_PARAMETERS_CHANGE_MSG = "RisConnectionParameterChange";
static constexpr auto RIS_PRESENTATION_CONTEXT_NOT_SET_MSG = "PresentationContextNotSet";
static constexpr auto RIS_INVALID_OPERATION_SPECIFIED = "InvalidOperationSpecified";



static constexpr auto MWL_QUERY_FAILED_MSG = "MwlQueryFailed";
static constexpr auto FAILED_TO_OPEN_DB_MSG = "FailedToOpenDatabase";
static constexpr auto MWL_ENTRY_NOT_FOUND_OR_QUERY_FAILED_MSG = "MwlEntryNotFoundOrQueryFailed";
static constexpr auto MWL_FAILED_TO_LOAD_ATTRIBUTES_MSG = "MwlFailedToLoadAttributes";
static constexpr auto MWL_FAILED_TO_START_TRANSACTION_MSG = "MwlFailedToStartTransaction";
static constexpr auto MWL_FAILED_TO_DELETE_ATTRIBUTES_MSG = "MwlFailedToDeleteAttribute";
static constexpr auto MWL_FAILED_TO_DELETE_ENTRIES_MSG = "MwlFailedToDeleteEntries";
static constexpr auto MWL_FAILED_TO_UPDATE_ENTRIES_MSG = "MwlFailedToUpdateEntries";
static constexpr auto MWL_FAILED_TO_COMMIT_TRANSACTION_MSG = "MwlFailedToCommitTransaction";
static constexpr auto MWL_FAILED_TO_INSERT_TAG_MSG = "MwlFailedToInsertTag";
static constexpr auto MWL_FAILED_TO_UPDATE_TAG_MSG = "MwlFailedToUpdateTag";
static constexpr auto MWL_FAILED_TO_INSERT_ATTRIBUTES_MSG = "MwlFailedToInsertAttribute";
static constexpr auto MWL_FAILED_TO_GET_ACTIVE_IDENTIFIERS_MSG = "MwlFailedToGetActiveIdentifiers";
static constexpr auto MWL_FAILED_TO_FIND_ENTRY_MSG = "MwlFailedToFindEntry";
static constexpr auto MWL_FAILED_TO_FIND_ACTIVE_IDENTIFIER_FOR_ENTRY_MSG = "MwlNoActiveIdentifierFoundForEntry";

static constexpr auto MWL_FAILED_TO_RETRIEVE_IDENTIFIER_ERROR = "MwlFailedToRetriveIdentifierList";

static constexpr auto LOG_DIR_CREATION_FAILED_ERROR = "LogDirCreationFailedError";
static constexpr auto LOG_DIR_NOT_EXIST_AFTER_CREATION_ERROR = "LogDirNotExistAfterCreationError";
static constexpr auto LOG_INSUFFICIENT_SPACE_ERROR = "LogInsufficientSpaceError";
static constexpr auto LOG_APPLOGGER_PROVIDER_NULL_ERROR = "AppLoggerProviderIsNullError";
static constexpr auto LOG_APPLOGGER_INSTANCE_IS_NULL = "LogFileCreationFailedError";
static constexpr auto LOG_FAILED_FOR_SERVICE = "LoggingFailedForService";
static constexpr auto LOG_EXCEPTION_FAILED_FOR_SERVICE = "LoggingExceptionFailedForService";
static constexpr auto LOG_FILE_START_LOGGER_INIT_MSG = "StartFileLoggerInit";
static constexpr auto LOG_FILE_LOGGER_INIT_SUCCEED_MSG = "FileLoggerInitSucceed";

static constexpr auto DEV_START_INIT_DEVICE_MSG = "StartDeviceInit";
static constexpr auto DEV_INIT_DEVICE_SUCCEED = "DeviceInitSucceed";



#endif // MESSAGEKEY_H
