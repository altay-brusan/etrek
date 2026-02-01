-- Migration: Create context_audit_log table
-- Date: 2026-01-31
-- Description: Adds audit trail table for session and workflow context changes
-- Database: MySQL

-- Drop table if exists (for clean migrations)
DROP TABLE IF EXISTS context_audit_log;

-- Create the context_audit_log table
CREATE TABLE context_audit_log (
    id INT AUTO_INCREMENT PRIMARY KEY,

    -- Context identification
    context_type ENUM('SESSION', 'WORKFLOW') NOT NULL,  -- Type of context
    context_key VARCHAR(100),                           -- NULL for session, workflow name for workflow (e.g., 'Examination')

    -- Event information
    event_type ENUM('CREATED', 'UPDATED', 'CLEARED') NOT NULL,  -- Type of event

    -- User information (captured at event time)
    user_id INT,                              -- NULL if no user context (e.g., system events)
    user_name VARCHAR(100),                   -- Username at time of event
    workstation_name VARCHAR(100),            -- Workstation identifier

    -- Context details as JSON for flexibility
    -- Session: {"username", "user_id", "workstation", "institution", "signin_time"}
    -- Workflow: {"worklist_entry_id", "patient_name", "patient_id", "accession_number", "body_part"}
    details JSON,

    -- Timestamp with microsecond precision
    event_timestamp DATETIME(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Create indexes for common query patterns

-- Index for querying by user (login history, user activity)
CREATE INDEX idx_context_audit_user_id ON context_audit_log(user_id);

-- Index for querying by context type (session events vs workflow events)
CREATE INDEX idx_context_audit_context_type ON context_audit_log(context_type);

-- Index for time-based queries (recent events, date range queries)
CREATE INDEX idx_context_audit_timestamp ON context_audit_log(event_timestamp);

-- Composite index for common filtered time queries
CREATE INDEX idx_context_audit_user_timestamp ON context_audit_log(user_id, event_timestamp);

-- Composite index for context type + timestamp queries
CREATE INDEX idx_context_audit_type_timestamp ON context_audit_log(context_type, event_timestamp);
